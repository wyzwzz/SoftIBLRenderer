#pragma once
#include "common.hpp"
#include "mesh.hpp"
#include "texture.hpp"
#include <omp.h>
class PBRShader;
class SkyShader;
class IShader
{
  public:
    virtual ~IShader()
    {
    }
    virtual Triangle vertexShader(const Triangle &inTriangle) const  = 0;
    virtual color4b fragmentShader(const float3 &inPos, const float3 &inNormal, const float2 &inTexCoord) const = 0;

    virtual const PBRShader* asPBRShader() const {return nullptr;}
    virtual const SkyShader* asSkyShader() const {return nullptr;}
};
inline color4b float3_to_color4b(const float3 &v)
{
    return color4b{std::clamp(v.x,0.f,1.f) * 255,
                   std::clamp(v.y,0.f,1.f) * 255,
                   std::clamp(v.z,0.f,1.f) * 255, 255};
}
class SkyShader: public IShader{
  public:
    mat4 model, view, projection, MVPMatrix;

    const MipMap2D<float3>* envMap;

    constexpr static float2 invAtan = vec2(0.1591,0.3183);
    float2 sampleSphericalMap(const float3& dir) const{
        float2 uv = float2(atan(dir.z,dir.x),asin(-dir.y));
        uv *= invAtan;//(-0.5,0.5)
        uv += 0.5f;//(0,1)
        return uv;
    }

    const SkyShader* asSkyShader() const override{ return this; }

    Triangle vertexShader(const Triangle &inTriangle) const override{
        Triangle outTriangle;
        mat4 rotView = mat4(mat3(view));
        for (int i = 0; i < 3; i++)
        {
            auto clipPos = projection * rotView * model * float4(inTriangle.vertices[i].pos, 1.f);
            outTriangle.vertices[i].gl_Position = {clipPos.x,clipPos.y,clipPos.w,clipPos.w};
            outTriangle.vertices[i].pos = model * float4(inTriangle.vertices[i].pos, 1.f);
            outTriangle.vertices[i].normal = model * float4(inTriangle.vertices[i].normal, 0.f);
            outTriangle.vertices[i].tex_coord = inTriangle.vertices[i].tex_coord;
        }
        return outTriangle;
    }
    color4b fragmentShader(const float3 &inPos, const float3 &inNormal, const float2 &inTexCoord) const override{

        float2 uv = sampleSphericalMap(normalize(inPos));
        auto env_color = LinearSampler::sample2D(envMap->get_level(0),uv.x,uv.y);
        return float3_to_color4b(env_color);
    }


};

class PBRShader : public IShader
{
  public:
    mat4 model, view, projection, MVPMatrix;
    const Texture<float3> *albedoMap;
    const Texture<float3> *normalMap;
    const Texture<float> *aoMap;
    const Texture<float> *roughnessMap;
    const Texture<float> *metallicMap;
    const MipMap2D<float3>* envMap;
    float3 viewPos;
    static constexpr int MaxLightNum = 4;
    int lightNum;
    float3 lightPos[MaxLightNum];
    float3 lightRadiance[MaxLightNum];
    static constexpr float PI = 3.14159265359f;
    // sRGB => XYZ => D65_2_D60 => AP1 => RRT_SAT
    static constexpr mat3 ACESInputMat =
        {
            {0.59719, 0.35458, 0.04823},
            {0.07600, 0.90834, 0.01566},
            {0.02840, 0.13383, 0.83777}
        };

    // ODT_SAT => XYZ => D60_2_D65 => sRGB
    static constexpr mat3 ACESOutputMat =
        {
            { 1.60475, -0.53108, -0.07367},
            {-0.10208,  1.10813, -0.00605},
            {-0.00327, -0.07276,  1.07602}
        };

    float3 RRTAndODTFit(float3 v) const
    {
        float3 a = v * (v + 0.0245786f) - 0.000090537f;
        float3 b = v * (0.983729f * v + 0.4329510f) + 0.238081f;
        return a / b;
    }

    float3 ACESFitted(float3 color) const
    {
        color = ACESInputMat * color;

        // Apply RRT and ODT
        color = RRTAndODTFit(color);

        color = ACESOutputMat * color;

        // Clamp to [0, 1]
        color = clamp(color,float3(0),float3(1));

        return color;
    }

    const PBRShader* asPBRShader() const { return this; }

    Triangle vertexShader(const Triangle &inTriangle) const override
    {
        Triangle outTriangle;
        for (int i = 0; i < 3; i++)
        {
            outTriangle.vertices[i].gl_Position = MVPMatrix * float4(inTriangle.vertices[i].pos, 1.f);
            outTriangle.vertices[i].pos = model * float4(inTriangle.vertices[i].pos, 1.f);
            outTriangle.vertices[i].normal = model * float4(inTriangle.vertices[i].normal, 0.f);
            outTriangle.vertices[i].tex_coord = inTriangle.vertices[i].tex_coord;
        }
        return outTriangle;
    }

    float DistributionGGX(const float3 &N, const float3 &H, float roughness) const
    {
        float a = roughness * roughness;
        float a2 = a * a;
        float NdotH = std::max(dot(N, H), 0.f);
        float NdotH2 = NdotH * NdotH;

        float nom = a2;
        float denom = (NdotH2 * (a2 - 1.f) + 1.f);
        denom = PI * denom * denom;
        return nom / denom;
    }
    float GeometrySchlickGGX(float NdotV, float roughtness) const
    {
        float r = roughtness + 1.f;
        float k = r * r * 0.125f;
        float nom = NdotV;
        float denom = NdotV * (1.f - k) + k;
        return nom / denom;
    }
    float GeometrySmith(float NdotV, float NdotL, float roughness) const
    {
        return GeometrySchlickGGX(NdotV, roughness) * GeometrySchlickGGX(NdotL, roughness);
    }
    float3 fresnelSchlick(float cosTheta, float3 F0) const
    {
        return F0 + (1.f - F0) * std::pow(std::max(1.f - cosTheta, 0.f), 5.f);
    }

    color4b fragmentShader(const float3 &inPos, const float3 &inNormal, const float2 &inTexCoord) const override
    {
        float3 albedo = LinearSampler::sample2D(*albedoMap, inTexCoord.x, inTexCoord.y);
        float metallic = LinearSampler::sample2D(*metallicMap, inTexCoord.x, inTexCoord.y);
        float roughness = LinearSampler::sample2D(*roughnessMap, inTexCoord.x, inTexCoord.y);
        float ao = LinearSampler::sample2D(*aoMap, inTexCoord.x, inTexCoord.y);
        float3 N = normalize(inNormal);
        float3 V = normalize(viewPos - inPos);
        float NdotV = std::max(dot(N, V), 0.f);
        float3 F0{0.04f, 0.04f, 0.04f};
        F0 = F0 * (1.f - metallic) + metallic * albedo;

        float3 Lo{0.f, 0.f, 0.f};

#pragma omp simd
        for (int i = 0; i < lightNum; i++)
        {
            float3 L = normalize(lightPos[i] - inPos);
            float3 H = normalize(V + L);
            float d2 = dot(lightPos[i] - inPos, lightPos[i] - inPos);
            float attenuation = 1.f / d2;
            float3 radiance = lightRadiance[i] * attenuation;
            float NdotL = std::max(dot(N, L), 0.f);
            float NDF = DistributionGGX(N, H, roughness);
            float G = GeometrySmith(NdotV, NdotL, roughness);
            float3 F = fresnelSchlick(std::max(dot(H, V), 0.f), F0);

            float3 numerator = NDF * G * F;
            float denominator = 4 * NdotV * NdotL + 0.001f;
            float3 specular = numerator / denominator;

            float3 kS = F;
            float3 kD = float3(1.f) - kS;
            kD *= 1.f - metallic;

            Lo += (kD * albedo / PI + specular) * radiance * NdotL;
        }

        float3 ambient = float3(0.03f) * albedo * ao;

        float3 color = ambient + Lo;

        //todo tone mapping
        color = ACESFitted(color);

        return float3_to_color4b(color);
    }
};