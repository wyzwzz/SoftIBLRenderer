#pragma once
#include "geometry.hpp"
#include "mesh.hpp"
#include "texture.hpp"
#include <memory>
class Model
{
  public:
    Model()
    {
    }
    Model(Model &&) noexcept;

    void loadMesh(const std::string &);
    void loadAlbedoMap(const std::string &);
    void loadNormalMap(const std::string &);
    void loadAOMap(const std::string &);
    void loadRoughnessMap(const std::string &);
    void loadMetallicMap(const std::string &);
    struct ModelTransform
    {
        float rotate_x, rotate_y, rotate_z;
        float scale_x, scale_y, scale_z;
        float transfer_x, transfer_y, transfer_z;
    };
    void loadModelMatrix(ModelTransform desc);

    Mesh *getMesh();
    mat4 getModelMatrix();
    Texture<float3> *getAlbedoMap();
    Texture<float3> *getNormalMap();
    Texture<float> *getAOMap();
    Texture<float> *getRoughnessMap();
    Texture<float> *getMetallicMap();
    void setModelMatrix(mat4);
    const BoundBox3D &getBoundBox() const;

  private:
    Texture<float3> albedo;
    Texture<float3> normal;
    Texture<float> ambientO;
    Texture<float> roughness;
    Texture<float> metallic;

    std::unique_ptr<Mesh> mesh;
    BoundBox3D box;
    mat4 model_matrix{1.f};
};
