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
    Texture<float3>* albedoMap;
    Texture<float3>* normalMap;
    Texture<float>* aoMap;
    Texture<float>* roughnessMap;
    Texture<float>* metallicMap;
    float3 viewPos;
    static constexpr int MaxLightNum = 4;
    int lightNum;
    float3 lightPos[MaxLightNum];
    float3 lightRadiance[MaxLightNum];
    static constexpr float PI = 3.14159265359f;
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
    color4b float3_to_color4b(const float3& v){
        return color4b{v.x*255,v.y*255,v.z*255,255};
    }
    float DistributionGGX(const float3& N,const float3& H,float roughness){
        float a = roughness * roughness;
        float a2 = a * a;
        float NdotH = std::max(dot(N,H),0.f);
        float NdotH2 = NdotH * NdotH;

        float nom = a2;
        float denom = (NdotH2 * (a2 - 1.f) + 1.f);
        denom = PI * denom * denom;
        return nom / denom;
    }
    float GeometrySchlickGGX(float NdotV,float roughtness){
        float r = roughtness + 1.f;
        float k = r * r * 0.125f;
        float nom = NdotV;
        float denom = NdotV * (1.f - k) + k;
        return nom / denom;
    }
    float GeometrySmith(float NdotV,float NdotL,float roughness){
        return GeometrySchlickGGX(NdotV,roughness) * GeometrySchlickGGX(NdotL,roughness);
    }
    float3 fresnelSchlick(float cosTheta,float3 F0){
        return F0 + (1.f - F0) * std::pow(std::max(1.f-cosTheta,0.f),5.f);
    }

    color4b fragmentShader(const float3& inPos,const float3& inNormal,const float2& inTexCoord) override{
        float3 albedo = LinearSampler::sample2D(*albedoMap,inTexCoord.x,inTexCoord.y);
        float metallic = LinearSampler::sample2D(*metallicMap,inTexCoord.x,inTexCoord.y);
        float roughness=  LinearSampler::sample2D(*roughnessMap,inTexCoord.x,inTexCoord.y);
        float ao = LinearSampler::sample2D(*aoMap,inTexCoord.x,inTexCoord.y);
        float3 N = normalize(inNormal);
        float3 V = normalize(viewPos - inPos);
        float NdotV = std::max(dot(N,V),0.f);
        float3 F0{0.04f,0.04f,0.04f};
        F0 = F0 * (1.f-metallic) + metallic * albedo;

        float3 Lo{0.f,0.f,0.f};

        for(int i = 0;i<lightNum;i++){
            float3 L = normalize(lightPos[i] - inPos);
            float3 H = normalize(V + L);
            float d2 = dot(lightPos[i]-inPos,lightPos[i]-inPos);
            float attenuation = 1.f / d2;
            float3 radiance = lightRadiance[i] * attenuation;
            float NdotL = std::max(dot(N,L),0.f);
            float NDF = DistributionGGX(N,H,roughness);
            float G = GeometrySmith(NdotV,NdotL,roughness);
            float3 F = fresnelSchlick(std::max(dot(H,V),0.f),F0);

            float3 numerator = NDF * G * F;
            float denominator = 4 * NdotV * NdotL + 0.001f;
            float3 specular = numerator / denominator;

            float3 kS = F;
            float3 kD = float3(1.f) - kS;
            kD *= 1.f - metallic;

            Lo += (kD*albedo/PI + specular) * radiance * NdotL;
        }

        float3 ambient = float3(0.03f) * albedo * ao;

        float3 color = ambient + Lo;

        color = color / (color+float3(1.f));

        return float3_to_color4b(color);
    }
};