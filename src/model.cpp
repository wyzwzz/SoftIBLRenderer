//
// Created by wyz on 2022/2/21.
//
#include "model.hpp"
#include <json.hpp>
#include <fstream>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <iostream>
auto LoadRGBImage(const std::string& path){
//    stbi_set_flip_vertically_on_load(true);
    int width,height,channels;
    auto data = stbi_load(path.c_str(),&width,&height,&channels,0);
    assert(channels==3);
    if(!data) throw std::runtime_error("load rgb image failed");
    Texture<float3> t(width,height);
    constexpr float inv = 1.f/255.f;
    for(int x = 0;x<width;x++){
        for(int y = 0;y<height;y++){
            t(x,y) = float3{static_cast<float>(data[(y*width+x)*channels+0])*inv,
                            static_cast<float>(data[(y*width+x)*channels+1])*inv,
                            static_cast<float>(data[(y*width+x)*channels+2])*inv};
        }
    }
    stbi_image_free(data);
    std::cout<<"successfully load "<<path<<std::endl;
    return t;
}
auto LoadSRGBImage(const std::string& path){
//    stbi_set_flip_vertically_on_load(true);
    int width,height,channels;
    auto data = stbi_load(path.c_str(),&width,&height,&channels,0);
    assert(channels==3);
    if(!data) throw std::runtime_error("load srgb image failed");
    Texture<float3> t(width,height);
    constexpr float inv = 1.f/255.f;
    for(int x = 0;x<width;x++){
        for(int y = 0;y<height;y++){
            t(x,y) = float3{std::pow(static_cast<float>(data[(y*width+x)*channels+0])*inv,2.2f),
                            std::pow(static_cast<float>(data[(y*width+x)*channels+1])*inv,2.2f),
                            std::pow(static_cast<float>(data[(y*width+x)*channels+2])*inv,2.2f)};
        }
    }
    stbi_image_free(data);
    std::cout<<"successfully load "<<path<<std::endl;
    return t;
}
auto LoadXYZImage(const std::string& path){
//    stbi_set_flip_vertically_on_load(true);
    int width,height,channels;
    auto data = stbi_load(path.c_str(),&width,&height,&channels,0);
    assert(channels==3);
    if(!data) throw std::runtime_error("load xyz image failed");
    Texture<float3> t(width,height);
    constexpr float inv = 2.f/255.f;
    for(int x = 0;x<width;x++){
        for(int y = 0;y<height;y++){
            t(x,y) = float3{static_cast<float>(data[(y*width+x)*channels+0])*inv-1.f,
                            static_cast<float>(data[(y*width+x)*channels+1])*inv-1.f,
                            static_cast<float>(data[(y*width+x)*channels+2])*inv-1.f};
        }
    }
    stbi_image_free(data);
    std::cout<<"successfully load "<<path<<std::endl;
    return t;
}
auto LoadRImage(const std::string& path){
//    stbi_set_flip_vertically_on_load(true);
    int width,height,channels;
    auto data = stbi_load(path.c_str(),&width,&height,&channels,0);
    assert(channels==1);
    if(!data) throw std::runtime_error("load r image failed");
    Texture<float> t(width,height);
    constexpr float inv = 1.f/255.f;
    for(int x = 0;x<width;x++){
        for(int y = 0;y<height;y++){
            t(x,y) = static_cast<float>(data[(y*width+x)*channels+0])*inv;
        }
    }
    stbi_image_free(data);
    std::cout<<"successfully load "<<path<<std::endl;
    return t;
}

auto LoadSRImage(const std::string& path){
//    stbi_set_flip_vertically_on_load(true);
    int width,height,channels;
    auto data = stbi_load(path.c_str(),&width,&height,&channels,0);
    assert(channels==1);
    if(!data) throw std::runtime_error("load r image failed");
    Texture<float> t(width,height);
    constexpr float inv = 1.f/255.f;
    for(int x = 0;x<width;x++){
        for(int y = 0;y<height;y++){
            t(x,y) = std::pow(static_cast<float>(data[(y*width+x)*channels+0])*inv,2.2f);
        }
    }
    stbi_image_free(data);
    std::cout<<"successfully load "<<path<<std::endl;
    return t;
}

Model::Model(const std::string &modelFile)
{
    std::ifstream in(modelFile);
    if(!in.is_open()){
        throw std::runtime_error("Failed to open model file");
    }
    nlohmann::json j;
    in >> j;
    in.close();
    std::string mesh_path = j.at("mesh");
    std::string albedo_path = j.at("albedo");
    std::string normal_path = j.at("normal");
    std::string ambient_path = j.at("ambient");
    std::string roughness_path = j.at("roughness");
    std::string metallic_path = j.at("metallic");
    this->mesh = std::make_unique<Mesh>(mesh_path);
    this->albedo = LoadSRGBImage(albedo_path);
    this->normal = LoadXYZImage(normal_path);
    this->ambientO = LoadRImage(ambient_path);
    this->roughness = LoadRImage(roughness_path);
    this->metallic = LoadRImage(metallic_path);
    this->model_matrix = mat4(1.f);
}
Mesh *Model::getMesh()
{
    return mesh.get();
}
mat4 Model::getModelMatrix()
{
    return model_matrix;
}
Texture<float3> *Model::getAlbedoMap()
{
    return &albedo;
}
Texture<float3> *Model::getNormalMap()
{
    return &normal;
}
Texture<float> *Model::getAOMap()
{
    return &ambientO;
}
Texture<float> *Model::getRoughnessMap()
{
    return &roughness;
}
Texture<float> *Model::getMetallicMap()
{
    return &metallic;
}
Model::Model(Model &&rhs) noexcept
:albedo(std::move(rhs.albedo)),normal(std::move(rhs.normal)),
      ambientO(std::move(rhs.ambientO)),
roughness(std::move(rhs.roughness)),metallic(std::move(rhs.metallic)),
      mesh(std::move(rhs.mesh)),model_matrix(rhs.model_matrix)
{

}

