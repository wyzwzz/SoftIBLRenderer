#pragma once
#include "geometry.hpp"
#include "mesh.hpp"
#include "texture.hpp"
#include <memory>
class Scene;

struct IBL{
    static constexpr int IrradianceMapSize = 32;
    static constexpr int PrefilterMapSize = 128;
    static constexpr int PrefilterSampleCount = 1024;
    static constexpr int BRDFLUTSize = 512;
    static constexpr int BRDFSampleCount = 1024;
    Texture<float3> irradiance_map;
    MipMap2D<float3> prefilter_map;
    Texture<float2> brdf_lut;
};
void createIBLResource(IBL& ibl,const MipMap2D<float3>& env_mipmap);
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
    void loadEnvironmentMap(const std::string&);

    struct ModelTransform
    {
        float rotate_x, rotate_y, rotate_z;
        float scale_x, scale_y, scale_z;
        float transfer_x, transfer_y, transfer_z;
    };
    void loadModelMatrix(ModelTransform desc);

    const Mesh *getMesh() const;
    mat4 getModelMatrix() const;
    const Texture<float3> *getAlbedoMap() const;
    const Texture<float3> *getNormalMap() const;
    const Texture<float> *getAOMap() const;
    const Texture<float> *getRoughnessMap() const;
    const Texture<float> *getMetallicMap() const;
    const std::shared_ptr<MipMap2D<float3>>& getEnvironmentMap() const;
    void setModelMatrix(mat4);
    const BoundBox3D &getBoundBox() const;
    const IBL& getIBL() const;

    friend class Scene;
  private:
    Texture<float3> albedo;
    Texture<float3> normal;
    Texture<float> ambientO;
    Texture<float> roughness;
    Texture<float> metallic;


    std::shared_ptr<MipMap2D<float3>> env_mipmap;
    IBL ibl;


    std::unique_ptr<Mesh> mesh;
    BoundBox3D box;
    mat4 model_matrix{1.f};
};