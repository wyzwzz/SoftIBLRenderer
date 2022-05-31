#pragma once

#include "camera.hpp"
#include "model.hpp"

class Scene
{
  public:
    Scene();

    void loadScene(const std::string &);

    void loadEnvMap(const std::string&);

    void addModel(Model model);

    void addLight(const Light &light);

    void setCamera(const Camera &camera);

    const std::vector<Model> &getModels();

    std::vector<Model *> getVisibleModels();

    const std::vector<Light> &getLights() const;

    const std::vector<Light> &getLights();

    const Model* getSkyBox() const;

    const Camera *getCamera() const;

    Camera* getCamera();


    void clearModels();

    void clearLights();

    void clearScene();

  private:
    std::vector<Model> models;

    std::vector<Light> lights;

    Box<Model> skybox;

    Camera camera;
};
