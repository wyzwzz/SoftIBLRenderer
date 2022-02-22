//
// Created by wyz on 2022/2/14.
//
#pragma once

#include "common.hpp"
#include <glm/gtc/matrix_transform.hpp>
class Camera{
  public:
    Camera(){
        position = float3{0.f,0.f,8.f};
        target = float3{0.f,0.f,0.f};
        front = float3{0.f,0.f,-1.f};
        up = float3{0.f,1.f,0.f};
        right = float3{1.f,0.f,0.f};
        fov = 20.f;
        aspect = static_cast<float>(ScreenWidth) / static_cast<float>(ScreenHeight);
        z_near = 0.1f;
        z_far = 50.f;
    }
    mat4 getViewMatrix() const;
    mat4 getProjMatrix() const;
    float3 position;
    float3 target;
    float3 front;
    float3 up;
    float3 right;
    float pitch{0.f};
    float yaw{-90.f};
    float fov;
    float aspect;
    float z_near,z_far;
};
inline mat4 Camera::getViewMatrix() const
{
    return glm::lookAt(position,target,up);
}
inline mat4 Camera::getProjMatrix() const
{
    return glm::perspective(glm::radians(fov*0.5f),aspect,z_near,z_far);
}
