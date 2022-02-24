//
// Created by wyz on 2022/2/15.
//
#pragma once
#include "model.hpp"
#include "camera.hpp"
#include "light.hpp"
#include <memory>
class Scene{
  public:
    Scene();
    const std::vector<Model>& getModels();
    std::vector<Model*> getVisibleModels();
    const std::vector<Light>& getLights();
    Camera* getCamera();
    void loadScene(const std::string&);
    void addModel(Model model);
    void addLight(const Light& light);
    void setCamera(const Camera& camera);
    void clearModels();
    void clearLights();
    void clearScene();

  private:
    std::vector<Model> models;
    Camera camera;
    std::vector<Light> lights;
};
