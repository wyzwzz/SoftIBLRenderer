//
// Created by wyz on 2022/2/15.
//
#include "scene.hpp"
#include <json.hpp>
#include <fstream>
#include <array>
#include <iostream>
const std::vector<Model> &Scene::getModels()
{
    return models;
}
std::vector<Model *> Scene::getVisibleModels()
{
    std::vector<Model*> ms;
    for(auto& model:models){
        ms.emplace_back(&model);
    }
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
    //init settings

}
const std::vector<Light> &Scene::getLights()
{
    return lights;
}
void Scene::addLight(const Light& light)
{
    this->lights.emplace_back(light);
}
void Scene::loadScene(const std::string &filename)
{
    std::ifstream in(filename);
    if(!in.is_open()){
        throw std::runtime_error("open scene file failed");
    }
    nlohmann::json j;
    in >> j;
    in.close();
    int model_count = j.at("model_count");
    for(int i = 0;i < model_count;i++){
        auto name = "model_"+std::to_string(i+1);
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
        if(model.find("transform")!=model.end()){
            auto model_transform = model.at("transform");
            std::array<float,3> rotate = model_transform.at("rotation");
            std::array<float,3> scale = model_transform.at("scale");
            std::array<float,3> transfer = model_transform.at("transfer");
            Model::ModelTransform t{
                rotate[0],rotate[1],rotate[2],
                scale[0],scale[1],scale[2],
                transfer[0],transfer[1],transfer[2]
            };
            load_model.loadModelMatrix(t);
        }
        else{
            load_model.setModelMatrix(mat4(1.f));
        }
        this->models.emplace_back(std::move(load_model));
    }
    int light_count = j.at("light_count");
    for(int i = 0; i < light_count; i++){
        auto name = "light_" + std::to_string(i+1);
        auto light = j.at(name);
        std::array<float,3> position = light.at("position");
        std::array<float,3> radiance = light.at("radiance");
        Light l{
            float3{position[0],position[1],position[2]},
            float3{radiance[0],radiance[1],radiance[2]}
        };
         addLight(l);
    }

}
