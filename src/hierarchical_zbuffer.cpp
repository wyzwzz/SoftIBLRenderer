#include <iostream>
#include <queue>
#include <vector>
//#include <omp.h>

#include "zbuffer.hpp"
#include "parallel.hpp"
#include "logger.hpp"

struct HierarchicalZBuffer::Impl
{
    Impl(int w, int h)
    {
        quad_tree = std::make_unique<QuadTree>();
        quad_tree->buildQuadTree(w, h);
    }

    ~Impl() = default;

    class QuadTree
    {
      public:
        struct QuadNode
        {
            QuadNode *kids[4];
            QuadNode *parent;
            int level;
            float depth;
            BoundBox2D box;
        };

        // build quad tree from bottom to top
        void buildQuadTree(int base_x, int base_y, int base_len = 1)
        {
            int leaf_num_x = (base_x + base_len - 1) / base_len;
            int leaf_num_y = (base_y + base_len - 1) / base_len;
            int leaf_count = leaf_num_x * leaf_num_y;
            using Array2D = std::vector<std::vector<QuadNode *>>;

            std::vector<std::vector<QuadNode *>> leaf_nodes;
            leaf_nodes.resize(leaf_num_y);
            for (int y = 0; y < leaf_num_y; y++)
            {
                leaf_nodes[y].resize(leaf_num_x);
                for (int x = 0; x < leaf_num_x; x++)
                {
                    auto node = new QuadNode();
                    node->box = BoundBox2D{
                        float2{static_cast<float>(x * base_len), static_cast<float>(y * base_len)},
                        float2{static_cast<float>((x + 1) * base_len), static_cast<float>((y + 1) * base_len)}};
                    node->level = 0;
                    node->depth = 1.f; // max depth for vertex after mvp is 1.f
                    leaf_nodes[y][x] = node;
                }
            }

            int last_level_x = leaf_num_x;
            int last_level_y = leaf_num_y;
            auto get_kid = [](const Array2D &nodes, int x, int y, int i, int j) -> QuadNode * {
                if (i < x && j < y){
                    return nodes[j][i];
                }
                else{
                    return nullptr;
                }
            };
            Array2D last_level_nodes = std::move(leaf_nodes);
            do
            {
                int level_x = (last_level_x + 1) / 2;
                int level_y = (last_level_y + 1) / 2;
                Array2D level_nodes;

                level_nodes.resize(level_y);
                for (int y = 0; y < level_y; y++)
                {
                    level_nodes[y].resize(level_x);
                    for (int x = 0; x < level_x; x++)
                    {
                        auto node = new QuadNode();
                        node->kids[0] = get_kid(last_level_nodes, last_level_x, last_level_y, x * 2, y * 2);
                        node->kids[1] = get_kid(last_level_nodes, last_level_x, last_level_y, x * 2 + 1, y * 2);
                        node->kids[2] = get_kid(last_level_nodes, last_level_x, last_level_y, x * 2, y * 2 + 1);
                        node->kids[3] = get_kid(last_level_nodes, last_level_x, last_level_y, x * 2 + 1, y * 2 + 1);
                        auto box = node->kids[0]->box;
                        for (int i = 1; i < 4; i++)
                        {
                            if (node->kids[i])
                            {
                                box = UnionBoundBox(box, node->kids[i]->box);
                            }
                        }
                        node->box = box;
                        for (int i = 0; i < 4; i++){
                            if (node->kids[i]){
                                node->kids[i]->parent = node;
                            }
                        }
                        node->level = node->kids[0]->level + 1;
                        node->depth = 1.f;
                        level_nodes[y][x] = node;
                    }
                }
                last_level_x = level_x;
                last_level_y = level_y;
                this->level_leaf_nodes.emplace_back(std::move(last_level_nodes));
                last_level_nodes = std::move(level_nodes);
            } while (last_level_x > 1 || last_level_y > 1);
            assert(last_level_x == 1 && last_level_y == 1);
            root = last_level_nodes[0][0];
            this->level_leaf_nodes.emplace_back(std::move(last_level_nodes));
            levels = root->level + 1;
            assert(root->parent == nullptr);
            LOG_DEBUG("root level: {}",root->level);
            LOG_DEBUG("quad tree hor hierarchical zbuffer build successfully");
        }

        void destroy()
        {
            std::queue<QuadNode *> q;
            q.push(root);
            while (!q.empty())
            {
                auto p = q.front();
                q.pop();
                if (!p)
                    continue;
                for (int i = 0; i < 4; i++)
                {
                    q.push(p->kids[i]);
                }
                delete p;
            }
        }

        // very cost time
        void clear(QuadNode *node)
        {
            if (!node)
                return;
            node->depth = 1.f;
            for (int i = 0; i < 4; i++)
            {
                clear(node->kids[i]);
            }
        }

        void clear()
        {
            //clear(root);
            for (int l = 0; l < level_leaf_nodes.size(); l++)
            {
                parallel_forrange(0,(int)level_leaf_nodes[l].size(),[&](int,int row){
                    for (int col = 0; col < level_leaf_nodes[l][row].size(); col++)
                    {
                        level_leaf_nodes[l][row][col]->depth = std::numeric_limits<float>::max();
                    }
                });
//#pragma omp parallel for
//                for (int row = 0; row < level_leaf_nodes[l].size(); row++)
//                {
//                    for (int col = 0; col < level_leaf_nodes[l][row].size(); col++)
//                    {
//                        level_leaf_nodes[l][row][col]->depth = std::numeric_limits<float>::max();
//                    }
//                }
            }
        }

        ~QuadTree()
        {
            destroy();
        }

        bool isLeafNode(QuadNode *node) const
        {
            return !node->kids[0];
        }

        QuadNode *root;

        int levels; // level count = max level + 1

        // for quickly clear because recursive clear root node can't use parallel speedup
        std::vector<std::vector<std::vector<QuadNode *>>> level_leaf_nodes;
    };

    Box<QuadTree> quad_tree;

    // test for entire triangle
    bool zTest(const BoundBox2D &box, float zVal) const
    {
        auto node = quad_tree->root;
        if (!BoundBox2DContainTest(node->box, box))
            return true;
        while (!quad_tree->isLeafNode(node))
        {
            if (node->depth < zVal)
                return false;
            int i;
            for (i = 0; i < 4; i++)
            {
                if (node->kids[i] && BoundBox2DContainTest(node->kids[i]->box, box))
                {
                    node = node->kids[i];
                    break;
                }
            }
            if (i == 4)
            {
                return true;
            }
        }
    }

    // for quick frag test
    bool zTest(int x, int y, float zVal) const
    {
        return quad_tree->level_leaf_nodes[0][y][x]->depth > zVal && zVal >= 0.f && zVal <= 1.f;
    }

    void updateZBuffer(int x, int y, float zVal)
    {
        auto leaf = quad_tree->level_leaf_nodes[0][y][x];
        leaf->depth = zVal;
        auto p = leaf->parent;
        while (p)
        {
            if (zVal > p->depth)
            {
                p->depth = zVal;
            }
            else
            {
                p->depth = std::max({p->kids[0] ? p->kids[0]->depth : 0.f, p->kids[1] ? p->kids[1]->depth : 0.f,
                                     p->kids[2] ? p->kids[2]->depth : 0.f, p->kids[3] ? p->kids[3]->depth : 0.f});
            }
            p = p->parent;
        }
    }

    void clear()
    {
        quad_tree->clear();
    }
};

HierarchicalZBuffer::HierarchicalZBuffer(int w, int h)
{
    impl = newBox<Impl>(w, h);
}

HierarchicalZBuffer::~HierarchicalZBuffer()
{

}

bool HierarchicalZBuffer::zTest(int x, int y, float zVal) const
{
    return impl->zTest(x, y, zVal);
}

void HierarchicalZBuffer::updateZBuffer(int x, int y, float zVal)
{
    impl->updateZBuffer(x, y, zVal);
}

void HierarchicalZBuffer::clear()
{
    impl->clear();
}

bool HierarchicalZBuffer::zTest(const BoundBox2D &box, float minZVal) const
{
    return impl->zTest(box, minZVal);
}
