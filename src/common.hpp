#pragma once

#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class IShader;
class SoftRenderer;
class Scene;
class Displayer;
class SoftRenderer;
class InputProcessor;
class Engine;


using namespace glm;
using float2 = glm::vec2;
using float3 = glm::vec3;

using float4 = glm::vec4;

using mat3 = glm::mat3;
using mat4 = glm::mat4;

using color4b = glm::vec<4, uint8_t>;

using color3b = glm::vec<3, uint8_t>;

constexpr int ScreenWidth = 1280;
constexpr int ScreenHeight = 720;
constexpr float ScreenAspectRatio = static_cast<float>(ScreenWidth) / static_cast<float>(ScreenHeight);

constexpr float PI = 3.14159265359f;


template<typename T>
using RC = std::shared_ptr<T>;

template<typename T, typename...Args>
RC<T> newRC(Args&&...args)
{
    return std::make_shared<T>(std::forward<Args>(args)...);
}

template<typename T>
using Box = std::unique_ptr<T>;

template<typename T, typename...Args>
Box<T> newBox(Args&&...args)
{
    return std::make_unique<T>(std::forward<Args>(args)...);
}

struct Light
{
    float3 light_position;
    float3 light_radiance;
};

constexpr float2 invAtan = vec2(0.1591,0.3183);

inline float2 sampleSphericalMap(const float3& dir) {
    float2 uv = float2(atan(dir.z,dir.x),asin(-dir.y));
    uv *= invAtan;//(-0.5,0.5)
    uv += 0.5f;//(0,1)
    return uv;
}
inline float3 sampleEquirectangularMap(const float2& uv){
    //local coord but world up is (0,1,0) not (0,0,1)
    float phi = uv.x * 2 * PI;
    float theta = uv.y * PI;
    float y = std::cos(theta);
    float x = std::sin(theta) * std::cos(phi);
    float z = std::sin(theta) * std::sin(phi);
    return {x,y,z};
}
inline void coordinate(const float3& v1,float3& v2,float3& v3){
    if(std::abs(v1.x) > std::abs(v1.y))
        v2 = float3(-v1.z,0,v1.x) / std::sqrt(v1.x*v1.x+v1.z*v1.z);
    else
        v2 = float3(0,v1.z,-v1.y) / std::sqrt(v1.y*v1.y+v1.z*v1.z);
    v3 = cross(v1,v2);
}
