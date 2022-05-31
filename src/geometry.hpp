#pragma once

#include <utility>
#include "common.hpp"

// Ax + By + Cz + D = 0
struct Plane
{
    float3 normal;
    float D;
};

enum class BoxVisibility
{
    Invisible,
    Intersecting,
    FullyVisible
};

struct Frustum
{
    enum PLANE_IDX : uint32_t
    {
        LEFT_PLANE_IDX = 0,
        RIGHT_PLANE_IDX = 1,
        BOTTOM_PLANE_IDX = 2,
        TOP_PLANE_IDX = 3,
        NEAR_PLANE_IDX = 4,
        FAR_PLANE_IDX = 5,
        NUM_PLANES = 6
    };

    Plane left_plane;
    Plane right_plane;
    Plane bottom_plane;
    Plane top_plane;
    Plane near_plane;
    Plane far_plane;

    const Plane &getPlane(PLANE_IDX Idx) const
    {

        const Plane *Planes = reinterpret_cast<const Plane *>(this);
        return Planes[static_cast<size_t>(Idx)];
    }

    Plane &getPlane(PLANE_IDX Idx)
    {
        Plane *Planes = reinterpret_cast<Plane *>(this);
        return Planes[static_cast<size_t>(Idx)];
    }
};

struct BoundBox3D
{
    float3 min_p;
    float3 max_p;
};
struct BoundBox2D
{
    float2 min_p;
    float2 max_p;
};
inline BoundBox2D UnionBoundBox(const BoundBox2D &b1, const BoundBox2D &b2)
{
    return BoundBox2D{{
                          std::min(b1.min_p.x, b2.min_p.x),
                          std::min(b1.min_p.y, b2.min_p.y),
                      },
                      {
                          std::max(b1.max_p.x, b2.max_p.x),
                          std::max(b1.max_p.y, b2.max_p.y),
                      }};
}

inline bool BoundBox2DIntersectTest(const BoundBox2D &b1, const BoundBox2D &b2)
{
    return std::max(b1.min_p.x, b2.min_p.x) < std::min(b1.max_p.x, b2.max_p.x) &&
           std::max(b1.min_p.y, b2.min_p.y) < std::min(b1.max_p.y, b2.max_p.y);
}
inline bool BoundBox2DContainTest(const BoundBox2D &b1, const BoundBox2D &b2)
{
    return b1.min_p.x <= b2.min_p.x && b1.min_p.y <= b2.min_p.y && b1.max_p.x >= b2.max_p.x && b1.max_p.y >= b2.max_p.y;
}

static BoxVisibility GetBoxVisibilityAgainstPlane(const Plane &plane, const BoundBox3D &box)
{
    const auto &normal = plane.normal;
    float3 max_point{(normal.x > 0.f) ? box.max_p.x : box.min_p.x, (normal.y > 0.f) ? box.max_p.y : box.min_p.y,
                     (normal.z > 0.f) ? box.max_p.z : box.min_p.z};
    float d_max = dot(max_point, normal) + plane.D;
    if (d_max < 0.f)
        return BoxVisibility::Invisible;
    float3 min_point{(normal.x > 0.f) ? box.min_p.x : box.max_p.x, (normal.y > 0.f) ? box.min_p.y : box.max_p.y,
                     (normal.z > 0.f) ? box.min_p.z : box.max_p.z};
    float d_min = dot(min_point, normal) + plane.D;
    if (d_min > 0.f)
        return BoxVisibility::FullyVisible;

    return BoxVisibility::Intersecting;
}

static BoxVisibility GetBoxVisibility(const Frustum &frustum, const BoundBox3D &box)
{
    uint32_t num_planes_inside = 0;
    for (uint32_t plane_idx = 0; plane_idx < Frustum::NUM_PLANES; plane_idx++)
    {
        const auto &cur_plane = frustum.getPlane(static_cast<Frustum::PLANE_IDX>(plane_idx));

        auto visibility_against_plane = GetBoxVisibilityAgainstPlane(cur_plane, box);

        if (visibility_against_plane == BoxVisibility::Invisible)
            return BoxVisibility::Invisible;

        if (visibility_against_plane == BoxVisibility::FullyVisible)
            num_planes_inside++;
    }
    return (num_planes_inside == Frustum::NUM_PLANES) ? BoxVisibility::FullyVisible : BoxVisibility::Intersecting;
}

inline bool FrustumIntersectWithBoundBox(const Frustum &frustum, const BoundBox3D &box)
{
    return GetBoxVisibility(frustum, box) != BoxVisibility::Invisible;
}
inline void ExtractFrustumFromProjViewMatrix(const mat4 &matrix, Frustum &frustum, bool is_OpenGL = true)
{
    // Left clipping plane
    frustum.left_plane.normal.x = matrix[0][3] + matrix[0][0];
    frustum.left_plane.normal.y = matrix[1][3] + matrix[1][0];
    frustum.left_plane.normal.z = matrix[2][3] + matrix[2][0];
    frustum.left_plane.D = matrix[3][3] + matrix[3][0];

    // Right clipping plane
    frustum.right_plane.normal.x = matrix[0][3] - matrix[0][0];
    frustum.right_plane.normal.y = matrix[1][3] - matrix[1][0];
    frustum.right_plane.normal.z = matrix[2][3] - matrix[2][0];
    frustum.right_plane.D = matrix[3][3] - matrix[3][0];

    // Top clipping plane
    frustum.top_plane.normal.x = matrix[0][3] - matrix[0][1];
    frustum.top_plane.normal.y = matrix[1][3] - matrix[1][1];
    frustum.top_plane.normal.z = matrix[2][3] - matrix[2][1];
    frustum.top_plane.D = matrix[3][3] - matrix[3][1];

    // Bottom clipping plane
    frustum.bottom_plane.normal.x = matrix[0][3] + matrix[0][1];
    frustum.bottom_plane.normal.y = matrix[1][3] + matrix[1][1];
    frustum.bottom_plane.normal.z = matrix[2][3] + matrix[2][1];
    frustum.bottom_plane.D = matrix[3][3] + matrix[3][1];

    // Near clipping plane
    if (is_OpenGL)
    {
        // -w <= z <= w
        frustum.near_plane.normal.x = matrix[0][3] + matrix[0][2];
        frustum.near_plane.normal.y = matrix[1][3] + matrix[1][2];
        frustum.near_plane.normal.z = matrix[2][3] + matrix[2][2];
        frustum.near_plane.D = matrix[3][3] + matrix[3][2];
    }
    else
    {
        // 0 <= z <= w
        frustum.near_plane.normal.x = matrix[0][2];
        frustum.near_plane.normal.y = matrix[1][2];
        frustum.near_plane.normal.z = matrix[2][2];
        frustum.near_plane.D = matrix[3][2];
    }

    // Far clipping plane
    frustum.far_plane.normal.x = matrix[0][3] - matrix[0][2];
    frustum.far_plane.normal.y = matrix[1][3] - matrix[1][3];
    frustum.far_plane.normal.z = matrix[2][3] - matrix[2][2];
    frustum.far_plane.D = matrix[3][3] - matrix[3][2];
}
