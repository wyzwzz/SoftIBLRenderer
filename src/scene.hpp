#pragma once
#include "camera.hpp"
#include "light.hpp"
#include "model.hpp"
#include <memory>
class Scene
{
  public:
    Scene();
    const std::vector<Model> &getModels();
    std::vector<Model *> getVisibleModels();
    const std::vector<Light> &getLights() const;
    const std::vector<Light> &getLights();
    const Model* getSkyBox() const;
    const Camera *getCamera() const;
    Camera* getCamera();
    void loadScene(const std::string &);
    void addModel(Model model);
    void addLight(const Light &light);
    void setCamera(const Camera &camera);
    void clearModels();
    void clearLights();
    void clearScene();
    void loadEnvMap(const std::string&);

  private:
    std::vector<Model> models;
    Camera camera;
    std::vector<Light> lights;
    std::unique_ptr<Model> skybox;
};
