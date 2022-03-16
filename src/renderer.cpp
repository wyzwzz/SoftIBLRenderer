#include "renderer.hpp"
#include "config.hpp"
#include "omp.h"
#include "rasterizer.hpp"
#include "shader.hpp"
#include <iostream>
SoftRenderer::SoftRenderer(const std::shared_ptr<Scene> &scene) : scene(scene)
{
    init();
}

void SoftRenderer::render()
{
    auto models = scene->getVisibleModels();
    if (!models.empty())
        std::cout << "render model count " << models.size() << std::endl;
    for (auto model : models)
    {
        PBRShader shader;
        shader.model = model->getModelMatrix();
        shader.view = scene->getCamera()->getViewMatrix();
        shader.projection = scene->getCamera()->getProjMatrix();
        shader.MVPMatrix = shader.projection * shader.view * shader.model;
        shader.albedoMap = model->getAlbedoMap();
        shader.normalMap = model->getNormalMap();
        shader.aoMap = model->getAOMap();
        shader.roughnessMap = model->getRoughnessMap();
        shader.metallicMap = model->getMetallicMap();
        shader.viewPos = scene->getCamera()->position;
        // set light
        {
            const auto &lights = scene->getLights();
            shader.lightNum = std::min(PBRShader::MaxLightNum, (int)lights.size());
            for (int i = 0; i < shader.lightNum; i++)
            {
                shader.lightPos[i] = lights[i].light_position;
                shader.lightRadiance[i] = lights[i].light_radiance;
            }
        }
        int triangle_count = model->getMesh()->triangles.size();
        std::cout << "render model triangle count " << triangle_count << std::endl;
        std::atomic<int> raster_count = 0;
#pragma omp parallel for firstprivate(shader) schedule(dynamic)
        for (int i = 0; i < triangle_count; i++)
        {
            const auto &triangle = model->getMesh()->triangles[i];

            if (backFaceCulling(triangle, shader.model))
                continue;

            auto triangle_primitive = shader.vertexShader(triangle);

            if (clipTriangle(triangle_primitive))
                continue;

            triangle_primitive.Homogenization();

            bool r = Rasterizer::rasterTriangle(triangle_primitive, shader, pixels, *z_buffer);
            if (r)
                raster_count++;
        }
        std::cout << "raster triangle count " << raster_count << std::endl;
    }
}

const Image<color4b> &SoftRenderer::getImage()
{
    return pixels;
}

void SoftRenderer::init()
{
    createFrameBuffer(ScreenWidth, ScreenHeight);
}
bool SoftRenderer::backFaceCulling(const Triangle &triangle, mat4 modelMatrix) const
{
    float3 e1 = normalize(triangle.vertices[1].pos - triangle.vertices[0].pos);
    float3 e2 = normalize(triangle.vertices[2].pos - triangle.vertices[1].pos);
    float3 face_normal = cross(e1, e2);
    face_normal = modelMatrix * float4(face_normal, 0.f);
    return dot(scene->getCamera()->front, face_normal) > 0.0001f;
}
bool SoftRenderer::clipTriangle(const Triangle &triangle) const
{
    int outside_count = 0;
    for (int i = 0; i < 3; i++)
    {
        const auto &v = triangle.vertices[i].gl_Position;
        bool inside = (-v.w <= v.x && v.x <= v.w) && (-v.w <= v.y && v.y <= v.w) &&
                      (0.f <= v.z && v.z <= v.w); // z should in (0,1) for camera at origin
        if (!inside)
            outside_count++;
    }
    return outside_count == 3;
}
void SoftRenderer::createFrameBuffer(int w, int h)
{
    pixels = Image<color4b>(w, h);
#if USE_HIERARCHICAL_Z_BUFFER
    z_buffer = std::make_unique<HierarchicalZBuffer>(w, h);
#else
    z_buffer = std::make_unique<NaiveZBuffer>(w, h);
#endif
}
void SoftRenderer::clearFrameBuffer()
{
    pixels.clear();
    z_buffer->clear();
}
