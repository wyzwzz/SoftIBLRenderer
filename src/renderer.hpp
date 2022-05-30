#pragma once
#include "common.hpp"
#include "scene.hpp"
#include "zbuffer.hpp"
#include <memory>
class IShader;
class SoftRenderer
{
  public:
    explicit SoftRenderer(const std::shared_ptr<Scene> &scene);

    void render();

    void render(const IShader& shader,const Model& model,bool clip = false);

    const Image<color4b> &getImage();

    bool backFaceCulling(const Triangle &triangle, mat4 modelMatrix) const;

    bool clipTriangle(const Triangle &triangle) const;

    void clearFrameBuffer();

    void createFrameBuffer(int w, int h);

  private:
    void init();

  private:
    std::shared_ptr<Scene> scene;
    Image<color4b> pixels;
    std::unique_ptr<ZBuffer> z_buffer;
};
