#include "scene.hpp"
#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <json.hpp>
const std::vector<Model> &Scene::getModels()
{
    return models;
}
std::vector<Model *> Scene::getVisibleModels()
{
    std::vector<Model *> ms;
    for (auto &model : models)
    {
        auto box = model.getBoundBox();
        auto model_matrix = model.getModelMatrix();
        float3 vs[8];
        vs[0] = {box.min_p.x, box.min_p.y, box.min_p.z};
        vs[1] = {box.min_p.x, box.min_p.y, box.max_p.z};
        vs[2] = {box.min_p.x, box.max_p.y, box.min_p.z};
        vs[3] = {box.min_p.x, box.max_p.y, box.max_p.z};
        vs[4] = {box.max_p.x, box.min_p.y, box.min_p.z};
        vs[5] = {box.max_p.x, box.min_p.y, box.max_p.z};
        vs[6] = {box.max_p.x, box.max_p.y, box.min_p.z};
        vs[7] = {box.max_p.x, box.max_p.y, box.max_p.z};
        for (int i = 0; i < 8; i++)
        {
            vs[i] = model_matrix * float4(vs[0], 1.f);
            for (int j = 0; j < 3; j++)
            {
                box.min_p[j] = std::min(box.min_p[j], vs[i][j]);
                box.max_p[j] = std::max(box.max_p[j], vs[i][j]);
            }
        }
        auto vp = camera.getProjMatrix() * camera.getViewMatrix();
        Frustum frustum;
        ExtractFrustumFromProjViewMatrix(vp, frustum);
        auto visibility = GetBoxVisibility(frustum, box);
        if (visibility != BoxVisibility::Invisible)
        {
            ms.emplace_back(&model);
        }
    }
    // sort by model center to camera distance in order to draw near model first
    std::sort(ms.begin(), ms.end(), [&](Model *m1, Model *m2) {
        auto pos = camera.position;
        auto p1 = m1->getBoundBox().min_p + m1->getBoundBox().max_p;
        auto p2 = m2->getBoundBox().min_p + m2->getBoundBox().max_p;
        p1 = m1->getModelMatrix() * float4(p1, 1.f);
        p2 = m2->getModelMatrix() * float4(p2, 1.f);
        return length(p1 - pos) < length(p2 - pos);
    });
    return ms;
}
Camera *Scene::getCamera()
{
    return &camera;
}
void Scene::addModel(Model model)
{
    models.emplace_back(std::move(model));
}
void Scene::setCamera(const Camera &camera)
{
    this->camera = camera;
}
void Scene::clearModels()
{
    this->models.clear();
}
void Scene::clearScene()
{
    clearModels();
    clearLights();
}
void Scene::clearLights()
{
    this->lights.clear();
}
Scene::Scene()
{

}
const std::vector<Light> &Scene::getLights()
{
    return lights;
}
void Scene::addLight(const Light &light)
{
    this->lights.emplace_back(light);
}
void Scene::loadScene(const std::string &filename)
{
    std::ifstream in(filename);
    if (!in.is_open())
    {
        throw std::runtime_error("open scene file failed");
    }
    nlohmann::json j;
    in >> j;
    in.close();
    int model_count = j.at("model_count");
    for (int i = 0; i < model_count; i++)
    {
        auto name = "model_" + std::to_string(i + 1);
        auto model = j.at(name);
        auto mesh_path = model.at("mesh");
        auto albedo_path = model.at("albedo");
        auto normal_path = model.at("normal");
        auto ambient_path = model.at("ambient");
        auto roughness_path = model.at("roughness");
        auto metallic_path = model.at("metallic");
        Model load_model;
        load_model.loadMesh(mesh_path);
        load_model.loadAlbedoMap(albedo_path);
        load_model.loadNormalMap(normal_path);
        load_model.loadAOMap(ambient_path);
        load_model.loadRoughnessMap(roughness_path);
        load_model.loadMetallicMap(metallic_path);
        if (model.find("transform") != model.end())
        {
            auto model_transform = model.at("transform");
            std::array<float, 3> rotate = model_transform.at("rotation");
            std::array<float, 3> scale = model_transform.at("scale");
            std::array<float, 3> transfer = model_transform.at("transfer");
            Model::ModelTransform t{rotate[0], rotate[1],   rotate[2],   scale[0],   scale[1],
                                    scale[2],  transfer[0], transfer[1], transfer[2]};
            load_model.loadModelMatrix(t);
        }
        else
        {
            load_model.setModelMatrix(mat4(1.f));
        }
        this->models.emplace_back(std::move(load_model));
    }
    int light_count = j.at("light_count");
    for (int i = 0; i < light_count; i++)
    {
        auto name = "light_" + std::to_string(i + 1);
        auto light = j.at(name);
        std::array<float, 3> position = light.at("position");
        std::array<float, 3> radiance = light.at("radiance");
        Light l{float3{position[0], position[1], position[2]}, float3{radiance[0], radiance[1], radiance[2]}};
        addLight(l);
    }
}
