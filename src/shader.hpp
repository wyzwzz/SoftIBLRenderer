//
// Created by wyz on 2022/2/15.
//
#pragma once
#include "common.hpp"
#include "mesh.hpp"
#include "texture.hpp"
class IShader{
  public:
    virtual ~IShader(){}
    virtual Triangle vertexShader(const Triangle& inTriangle) = 0;
    virtual color4b fragmentShader(const float3& inPos,const float3& inNormal,const float2& inTexCoord) = 0;
};

class PBRShader: public IShader{
  public:
    mat4 model,view,projection,MVPMatrix;

    Triangle vertexShader(const Triangle& inTriangle) override{
        Triangle outTriangle;
        for(int i = 0;i < 3; i++){
            outTriangle.vertices[i].gl_Position = MVPMatrix*float4(inTriangle.vertices[i].pos,1.f);
            outTriangle.vertices[i].pos = model * float4(inTriangle.vertices[i].pos,1.f);
            outTriangle.vertices[i].normal = model * float4(inTriangle.vertices[i].normal,0.f);
            outTriangle.vertices[i].tex_coord = inTriangle.vertices[i].tex_coord;
        }
        return outTriangle;
    }
    color4b fragmentShader(const float3& inPos,const float3& inNormal,const float2& inTexCoord) override{
        return color4b{inNormal.x*255,inNormal.y*255,inNormal.z*255,255};
    }
};