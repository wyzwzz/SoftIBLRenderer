//
// Created by wyz on 2022/2/15.
//
#include "scene.hpp"
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
const Camera *Scene::getCamera()
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
