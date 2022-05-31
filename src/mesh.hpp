#pragma once

#include <string>
#include <vector>

#include "common.hpp"

struct Triangle
{
    struct Vertex
    {
        float4 gl_Position;
        float3 pos;
        float3 normal;
        float2 tex_coord;
    };

    Vertex vertices[3];

    void Homogenization()
    {
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                vertices[i].gl_Position[j] /= vertices[i].gl_Position.w;
            }
        }
    }
};

struct Mesh
{
    Mesh() = default;

    explicit Mesh(const std::string &path);

    std::vector<Triangle> triangles;
};