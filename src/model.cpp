#include "model.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include <iostream>
#include <stb_image.h>
auto LoadRGBImage(const std::string &path)
{
    stbi_set_flip_vertically_on_load(true);
    int width, height, channels;
    auto data = stbi_load(path.c_str(), &width, &height, &channels, 0);
    assert(channels == 3);
    if (!data)
        throw std::runtime_error("load rgb image failed");
    Texture<float3> t(width, height);
    constexpr float inv = 1.f / 255.f;
    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            t(x, y) = float3{static_cast<float>(data[(y * width + x) * channels + 0]) * inv,
                             static_cast<float>(data[(y * width + x) * channels + 1]) * inv,
                             static_cast<float>(data[(y * width + x) * channels + 2]) * inv};
        }
    }
    stbi_image_free(data);
    std::cout << "successfully load " << path << std::endl;
    return t;
}
auto LoadSRGBImage(const std::string &path)
{
    stbi_set_flip_vertically_on_load(true);
    int width, height, channels;
    auto data = stbi_load(path.c_str(), &width, &height, &channels, 0);
    assert(channels == 3);
    if (!data)
        throw std::runtime_error("load srgb image failed");
    Texture<float3> t(width, height);
    constexpr float inv = 1.f / 255.f;
    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            t(x, y) = float3{std::pow(static_cast<float>(data[(y * width + x) * channels + 0]) * inv, 2.2f),
                             std::pow(static_cast<float>(data[(y * width + x) * channels + 1]) * inv, 2.2f),
                             std::pow(static_cast<float>(data[(y * width + x) * channels + 2]) * inv, 2.2f)};
        }
    }
    stbi_image_free(data);
    std::cout << "successfully load " << path << std::endl;
    return t;
}
auto LoadXYZImage(const std::string &path)
{
    stbi_set_flip_vertically_on_load(true);
    int width, height, channels;
    auto data = stbi_load(path.c_str(), &width, &height, &channels, 0);
    assert(channels == 3);
    if (!data)
        throw std::runtime_error("load xyz image failed");
    Texture<float3> t(width, height);
    constexpr float inv = 2.f / 255.f;
    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            t(x, y) = float3{static_cast<float>(data[(y * width + x) * channels + 0]) * inv - 1.f,
                             static_cast<float>(data[(y * width + x) * channels + 1]) * inv - 1.f,
                             static_cast<float>(data[(y * width + x) * channels + 2]) * inv - 1.f};
        }
    }
    stbi_image_free(data);
    std::cout << "successfully load " << path << std::endl;
    return t;
}
auto LoadRImage(const std::string &path)
{
    stbi_set_flip_vertically_on_load(true);
    int width, height, channels;
    auto data = stbi_load(path.c_str(), &width, &height, &channels, 0);
    assert(channels == 1);
    if (!data)
        throw std::runtime_error("load r image failed");
    Texture<float> t(width, height);
    constexpr float inv = 1.f / 255.f;
    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            t(x, y) = static_cast<float>(data[(y * width + x) * channels + 0]) * inv;
        }
    }
    stbi_image_free(data);
    std::cout << "successfully load " << path << std::endl;
    return t;
}

auto LoadSRImage(const std::string &path)
{
    stbi_set_flip_vertically_on_load(true);
    int width, height, channels;
    auto data = stbi_load(path.c_str(), &width, &height, &channels, 0);
    assert(channels == 1);
    if (!data)
        throw std::runtime_error("load r image failed");
    Texture<float> t(width, height);
    constexpr float inv = 1.f / 255.f;
    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            t(x, y) = std::pow(static_cast<float>(data[(y * width + x) * channels + 0]) * inv, 2.2f);
        }
    }
    stbi_image_free(data);
    std::cout << "successfully load " << path << std::endl;
    return t;
}

auto LoadHDR(const std::string& path){
    stbi_set_flip_vertically_on_load(false);
    int w,h,nComp;
    auto d = stbi_loadf(path.c_str(),&w,&h,&nComp,0);
    if(!d){
        throw std::runtime_error("load image failed");
    }
    if(nComp == 3){
        return Image2D<float3>(w,h,reinterpret_cast<float3*>(d));
    }
    else if(nComp == 4){
        Image2D<float3> image(w,h);
        auto p = image.data();
        for(int i = 0; i < w * h; ++i){
            p[i] = {d[i*3],d[i*3+1],d[i*3+2]};
        }
        return image;
    }
    else{
        throw std::runtime_error("invalid image component");
    }
}

const Mesh *Model::getMesh() const
{
    return mesh.get();
}
mat4 Model::getModelMatrix() const
{
    return model_matrix;
}
const Texture<float3> *Model::getAlbedoMap() const
{
    return &albedo;
}
const Texture<float3> *Model::getNormalMap() const
{
    return &normal;
}
const Texture<float> *Model::getAOMap() const
{
    return &ambientO;
}
const Texture<float> *Model::getRoughnessMap() const
{
    return &roughness;
}
const Texture<float> *Model::getMetallicMap() const
{
    return &metallic;
}
Model::Model(Model &&rhs) noexcept
    : albedo(std::move(rhs.albedo)), normal(std::move(rhs.normal)), ambientO(std::move(rhs.ambientO)),
      roughness(std::move(rhs.roughness)), metallic(std::move(rhs.metallic)), mesh(std::move(rhs.mesh)),
      model_matrix(rhs.model_matrix)
{
}
void Model::setModelMatrix(mat4 m)
{
    this->model_matrix = m;
}
void Model::loadModelMatrix(ModelTransform desc)
{
    auto m1 = rotate(mat4(1.f), radians(desc.rotate_x), float3{1.f, 0.f, 0.f});
    auto m2 = rotate(mat4(1.f), radians(desc.rotate_y), float3{0.f, 1.f, 0.f});
    auto m3 = rotate(mat4(1.f), radians(desc.rotate_z), float3{0.f, 0.f, 1.f});
    auto s = glm::scale(mat4(1.f), float3{desc.scale_x, desc.scale_y, desc.scale_z});
    auto t = glm::translate(mat4(1.f), float3{desc.transfer_x, desc.transfer_y, desc.transfer_z});
    this->model_matrix = t * s * m3 * m2 * m1 * this->model_matrix;
}
void Model::loadMesh(const std::string &mesh_path)
{
    this->mesh = std::make_unique<Mesh>(mesh_path);
    box.min_p = {std::numeric_limits<float>::max(), std::numeric_limits<float>::max(),
                 std::numeric_limits<float>::max()};
    box.max_p = {-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max(),
                 -std::numeric_limits<float>::max()};
    for (auto &tri : mesh->triangles)
    {
        for (auto &v : tri.vertices)
        {
            box.min_p.x = std::min(box.min_p.x, v.pos.x);
            box.min_p.y = std::min(box.min_p.y, v.pos.y);
            box.min_p.z = std::min(box.min_p.z, v.pos.z);
            box.max_p.x = std::max(box.max_p.x, v.pos.x);
            box.max_p.y = std::max(box.max_p.y, v.pos.y);
            box.max_p.z = std::max(box.max_p.z, v.pos.z);
        }
    }
}
void Model::loadAlbedoMap(const std::string &albedo_path)
{
    this->albedo = LoadSRGBImage(albedo_path);
}
void Model::loadNormalMap(const std::string &normal_path)
{
    this->normal = LoadXYZImage(normal_path);
}
void Model::loadAOMap(const std::string &ambient_path)
{
    this->ambientO = LoadRImage(ambient_path);
}
void Model::loadRoughnessMap(const std::string &roughness_path)
{
    this->roughness = LoadRImage(roughness_path);
}
void Model::loadMetallicMap(const std::string &metallic_path)
{
    this->metallic = LoadRImage(metallic_path);
}
const BoundBox3D &Model::getBoundBox() const
{
    return box;
}
void Model::loadEnvironmentMap(const std::string& path){

    auto hdr = LoadHDR(path);
    this->env_mipmap = std::make_shared<MipMap2D<float3>>();
    this->env_mipmap->generate(hdr);
    std::cout<<"load and generate environment map successfully"<<std::endl;
}
const std::shared_ptr<MipMap2D<float3>>& Model::getEnvironmentMap() const
{
    return env_mipmap;
}
