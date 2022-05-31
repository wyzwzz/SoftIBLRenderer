#ifdef USE_OMP
#include <omp.h>
#endif
#include "config.hpp"
#include "parallel.hpp"
#include "logger.hpp"
#include "renderer.hpp"
#include "rasterizer.hpp"
#include "shader.hpp"
#include "model.hpp"

SoftRenderer::SoftRenderer(const std::shared_ptr<Scene> &scene) : scene(scene)
{
    init();
}

void SoftRenderer::render(const IShader &shader,const Model& model,bool clip)
{
    int triangle_count = model.getMesh()->triangles.size();

    LOG_DEBUG("render model triangle count: {}",triangle_count);
#ifndef NDEBUG
    std::atomic<int> raster_count = 0;
#endif
#ifndef USE_OMP
    parallel_forrange(0,triangle_count,[&](int,int i){
        const auto &triangle = model.getMesh()->triangles[i];

        if (backFaceCulling(triangle, model.getModelMatrix()))
            return;

        auto triangle_primitive = shader.vertexShader(triangle);

        if (clip && clipTriangle(triangle_primitive))
            return;

        triangle_primitive.Homogenization();

        bool r = Rasterizer::rasterTriangle(triangle_primitive, shader, pixels, *z_buffer);
#ifndef NDEBUG
        if (r)
            raster_count++;
#endif
    });
#else
#pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < triangle_count; i++)
    {
        const auto &triangle = model.getMesh()->triangles[i];

        if (backFaceCulling(triangle, model.getModelMatrix()))
            continue;

        auto triangle_primitive = shader.vertexShader(triangle);

        if (clip && clipTriangle(triangle_primitive))
            continue;

        triangle_primitive.Homogenization();

        bool r = Rasterizer::rasterTriangle(triangle_primitive, shader, pixels, *z_buffer);
#ifndef NDEBUG
        if (r)
            raster_count++;
#endif
    }
#endif

#ifndef NDEBUG
    LOG_DEBUG("raster triangle count: {}",raster_count);
#endif

}
void SoftRenderer::render()
{
    auto models = scene->getVisibleModels();

    if (!models.empty()){
        LOG_DEBUG("render models count: {}",models.size());
    }

    for (auto model : models)
    {
        PBRShader shader;
        shader.model        = model->getModelMatrix();
        shader.view         = scene->getCamera()->getViewMatrix();
        shader.projection   = scene->getCamera()->getProjMatrix();
        shader.MVPMatrix    = shader.projection * shader.view * shader.model;
        shader.albedoMap    = model->getAlbedoMap();
        shader.normalMap    = model->getNormalMap();
        shader.aoMap        = model->getAOMap();
        shader.roughnessMap = model->getRoughnessMap();
        shader.metallicMap  = model->getMetallicMap();
        shader.viewPos      = scene->getCamera()->position;
        // set light
        const auto &lights = scene->getLights();
        shader.lightNum = std::min(PBRShader::MaxLightNum, (int)lights.size());
        for (int i = 0; i < shader.lightNum; i++)
        {
            shader.lightPos[i] = lights[i].light_position;
            shader.lightRadiance[i] = lights[i].light_radiance;
        }

        int triangle_count = model->getMesh()->triangles.size();
        LOG_DEBUG("render model triangle count: {}",triangle_count);
#ifndef NDEBUG
        std::atomic<int> raster_count = 0;
#endif
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
#ifndef NDEBUG
            if (r)
                raster_count++;
#endif
        }
#ifndef NDEBUG
        LOG_DEBUG("raster triangle count: {}",raster_count);
#endif
    }
}

const Image<color4b> &SoftRenderer::getImage() const
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

bool use_hz = false;

void SoftRenderer::createFrameBuffer(int w, int h)
{
    pixels = Image<color4b>(w, h);
    if(use_hz){
        z_buffer = std::make_unique<HierarchicalZBuffer>(w, h);
        LOG_INFO("create hierarchical zbuffer");
    }
    else{
        z_buffer = std::make_unique<NaiveZBuffer>(w, h);
        LOG_INFO("create naive zbuffer");
    }
}

void SoftRenderer::clearFrameBuffer()
{
    pixels.clear();
    z_buffer->clear();
}

