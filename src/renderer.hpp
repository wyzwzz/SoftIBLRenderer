//
// Created by wyz on 2022/2/14.
//
#pragma once
#include "common.hpp"
#include "zbuffer.hpp"
#include "scene.hpp"
#include <memory>
class SoftRenderer{
  public:
    explicit SoftRenderer(const std::shared_ptr<Scene>& scene);

    void render();

    const Image<color4b>& getImage();

    bool backFaceCulling(const Triangle& triangle,mat4 modelMatrix) const;

    bool clipTriangle(const Triangle& triangle) const;

    void clearFrameBuffer();

    void createFrameBuffer(int w,int h);
  private:
    void init();
  private:
    std::shared_ptr<Scene> scene;
    Image<color4b> pixels;
    std::unique_ptr<ZBuffer> z_buffer;
};
