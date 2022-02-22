//
// Created by wyz on 2022/2/15.
//
#pragma once
#include "common.hpp"
#include <vector>
#include <string>
class Triangle{
  public:
    struct Vertex{
        float4 gl_Position;
        float3 pos;
        float3 normal;
        float2 tex_coord;
    };
    Vertex vertices[3];
    void Homogenization(){
        for(int i = 0;i<3;i++){
            for(int j = 0;j<3;j++){
                vertices[i].gl_Position[j] /= vertices[i].gl_Position.w;
            }
        }
    }
};

class Mesh{
  public:
    explicit Mesh(const std::string& path);
    std::vector<Triangle> triangles;
};