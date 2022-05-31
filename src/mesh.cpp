#include <iostream>

#include "mesh.hpp"
#include "logger.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

Mesh::Mesh(const std::string &path)
{
    tinyobj::ObjReader reader;
    if (!reader.ParseFromFile(path))
    {
        throw std::runtime_error(reader.Error());
    }

    auto &attrib = reader.GetAttrib();
    auto &shapes = reader.GetShapes();

    for (auto &shape : shapes)
    {
        for (auto face_vertex_count : shape.mesh.num_face_vertices)
        {
            if (face_vertex_count != 3)
            {
                throw std::runtime_error("invalid obj face vertex count: " + std::to_string(+face_vertex_count));
            }
        }
        if (shape.mesh.indices.size() % 3 != 0)
        {
            throw std::runtime_error("invalid obj index count: " + std::to_string(shape.mesh.indices.size()));
        }

        const size_t triangle_count = shape.mesh.indices.size() / 3;
        const size_t vertex_count   = attrib.vertices.size() / 3;

        LOG_INFO("shape ({}) triangle count: {}, vertex count: {}",shape.name,triangle_count,vertex_count);

        for (size_t i = 0; i < triangle_count; i++)
        {
            Triangle triangle{};
            for (int k = 0; k < 3; k++)
            {
                auto index = shape.mesh.indices[i * 3 + k];
                triangle.vertices[k].pos = {attrib.vertices[3 * index.vertex_index + 0],
                                            attrib.vertices[3 * index.vertex_index + 1],
                                            attrib.vertices[3 * index.vertex_index + 2]};

                triangle.vertices[k].normal = {attrib.normals[3 * index.normal_index + 0],
                                               attrib.normals[3 * index.normal_index + 1],
                                               attrib.normals[3 * index.normal_index + 2]};

                triangle.vertices[k].tex_coord = {attrib.texcoords[2 * index.texcoord_index + 0],
                                                  attrib.texcoords[2 * index.texcoord_index + 1]};
            }
            this->triangles.emplace_back(triangle);
        }
    }
    LOG_INFO("successfully load: {}",path);
}
