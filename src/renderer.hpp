#pragma once

#include "common.hpp"
#include "scene.hpp"
#include "zbuffer.hpp"

class SoftRenderer
{
  public:
    explicit SoftRenderer(const std::shared_ptr<Scene> &scene);

    [[deprecated]] void render();

    void render(const IShader& shader,const Model& model,bool clip = false);

    const Image<color4b> &getImage() const;

    bool backFaceCulling(const Triangle &triangle, mat4 modelMatrix) const;

    bool clipTriangle(const Triangle &triangle) const;

    void clearFrameBuffer();

    void createFrameBuffer(int w, int h);

  private:

    void init();

  private:

    RC<Scene> scene;

    Image<color4b> pixels;

    Box<ZBuffer> z_buffer;
};
