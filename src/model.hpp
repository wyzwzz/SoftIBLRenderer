//
// Created by wyz on 2022/2/15.
//
#pragma once
#include "mesh.hpp"
#include "texture.hpp"
#include <memory>
class Model{
  public:
    explicit Model(const std::string& modelFile);
    Model(Model&&) noexcept;
    Mesh* getMesh();
    mat4 getModelMatrix();
    Texture<float3>* getAlbedoMap();
    Texture<float3>* getNormalMap();
    Texture<float>*getAOMap();
    Texture<float>* getRoughnessMap();
    Texture<float>* getMetallicMap();
  private:
    Texture<float3> albedo;
    Texture<float3> normal;
    Texture<float> ambientO;
    Texture<float> roughness;
    Texture<float> metallic;

    std::unique_ptr<Mesh> mesh;
    mat4 model_matrix;
};

