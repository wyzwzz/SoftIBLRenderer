#include "model.hpp"
#include "parallel.hpp"
#include "logger.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace{
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

    auto LoadHDR(const std::string &path)
    {
        stbi_set_flip_vertically_on_load(false);
        int w, h, nComp;
        auto d = stbi_loadf(path.c_str(), &w, &h, &nComp, 0);
        if (!d)
        {
            throw std::runtime_error("load image failed");
        }
        if (nComp == 3)
        {
            return Image2D<float3>(w, h, reinterpret_cast<float3 *>(d));
        }
        else if (nComp == 4)
        {
            Image2D<float3> image(w, h);
            auto p = image.data();
            for (int i = 0; i < w * h; ++i)
            {
                p[i] = {d[i * 3], d[i * 3 + 1], d[i * 3 + 2]};
            }
            return image;
        }
        else
        {
            throw std::runtime_error("invalid image component");
        }
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
    LOG_INFO("mesh boundary: ({},{},{}) ~ ({},{},{})",box.min_p.x,box.min_p.y,box.min_p.z,box.max_p.x,box.max_p.y,box.max_p.z);
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
    LOG_INFO("load and generate environment map successfully");
}

const std::shared_ptr<MipMap2D<float3>>& Model::getEnvironmentMap() const
{
    return env_mipmap;
}

float RadicalInverse_Vdc(uint32_t bits){
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

float2 Hammersley(uint32_t i, uint32_t N){
    return float2(float(i)/float(N), RadicalInverse_Vdc(i));
}

float DistributionGGX(vec3 N, vec3 H, float roughness){
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = std::max(dot(N, H), 0.f);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

float3 ImportanceSampleGGX(float2 Xi, float3 N, float roughness){
    float a = roughness * roughness;

    float phi      = 2.f * PI * Xi.x; // phi是xy平面内与x轴的夹角
    float cosTheta = sqrt((1.f - Xi.y) / (1.f + (a*a - 1.f) * Xi.y));
    float sinTheta = sqrt(1.f - cosTheta * cosTheta);

    float3 H;
    H.x = cos(phi) * sinTheta;
    H.y = sin(phi) * sinTheta;
    H.z = cosTheta;

    float3 s,t;
    coordinate(N,s,t);

    //transform from local to world
    float3 sample_vec =  H.x * s + H.y * t + H.z * N;
    return normalize(sample_vec);
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    // note that we use a different k for IBL
    // 与直接光渲染的k计算不同
    float a = roughness;
    float k = (a * a) / 2.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
    float NdotV = std::max(dot(N, V), 0.f);
    float NdotL = std::max(dot(N, L), 0.f);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

void createIBLResource(IBL& ibl,const MipMap2D<float3>& env_mipmap)
{
    static float3 world_up = float3(0,1,0);
    float sample_delta = 0.025f;

    int irradiance_map_w = IBL::IrradianceMapSize;
    int irradiance_map_h = IBL::IrradianceMapSize;
    ibl.irradiance_map = Image2D<float3>(irradiance_map_w,irradiance_map_h);
    parallel_forrange(0,irradiance_map_h,[&](int,int h){
        for(int w = 0; w < irradiance_map_w; ++w){
            float u = (w + 0.5f) / irradiance_map_w;
            float v = (h + 0.5f) / irradiance_map_h;
            auto N = sampleEquirectangularMap({u,v});

            float3 s,t;
            coordinate(N,s,t);

            int sample_count = 0;
            float3 irradiance(0);

            for(float phi = 0.f; phi < 2.f * PI; phi += sample_delta){
                for(float theta = 0.f; theta < 0.5f * PI; theta += sample_delta){
                    float3 local_dir = float3(
                        std::sin(theta)*std::cos(phi),
                        std::sin(theta)*std::sin(phi),
                        std::cos(theta));
                    float3 world_dir = normalize(local_dir.x * s + local_dir.y * t + local_dir.z * N);

                    auto uv = sampleSphericalMap(world_dir);
                    irradiance += LinearSampler::sample2D(env_mipmap,uv.x,uv.y,0);
                    ++sample_count;
                }
            }
            irradiance /= static_cast<float>(sample_count);
            ibl.irradiance_map.at(w,h) = irradiance;
        }
    });
    LOG_INFO("finish generate irradiance map");

    int prefilter_map_w = IBL::PrefilterMapSize;
    int prefilter_map_h = IBL::PrefilterMapSize;
    int prefilter_sample_count = IBL::PrefilterSampleCount;
    ibl.prefilter_map.generate(prefilter_map_w,prefilter_map_h);
    LOG_INFO("prefilter map levels: {}",ibl.prefilter_map.levels());
    int mip_levels = ibl.prefilter_map.levels();
    for(int i = 0; i < mip_levels; ++i){
        float roughness = i * 1.f / (mip_levels - 1);
        int cur_prefilter_map_h = prefilter_map_h >> i;
        int cur_prefilter_map_w = prefilter_map_w >> i;
        parallel_forrange(0,cur_prefilter_map_h,[&](int,int h){
            for(int w = 0; w < cur_prefilter_map_w; ++w){
                float u = (w + 0.5f) / cur_prefilter_map_w;
                float v = (h + 0.5f) / cur_prefilter_map_h;

                auto N = sampleEquirectangularMap({u,v});
                float3 R = N;
                float3 V = R;

                float3 prefilter_color = float3(0);
                float total_weight = 0.f;
                for(int i = 0; i < prefilter_sample_count; ++i){
                    float2 xi = Hammersley(i,prefilter_sample_count);
                    float3 H  = ImportanceSampleGGX(xi,N,roughness);
                    float3 L  = normalize(2.f * dot(V,H) * H - V);

                    float NdotL = std::max(dot(N,L),0.f);

                    if(NdotL > 0.f){
                        auto uv = sampleSphericalMap(L);
                        //prefilter_color += LinearSampler::sample2D(*env_mipmap, uv.x,uv.y,0) * NdotL;
                        //total_weight += NdotL;

                        float D     = DistributionGGX(N,H,roughness);
                        float NdotH = std::max(dot(N,H),0.f);
                        float HdotV = std::max(dot(H,V),0.f);
                        float pdf   = D * NdotH / (4.f * HdotV) + 0.0001f;

                        float sa_texel  = 4.f * PI / (6.f * cur_prefilter_map_w * cur_prefilter_map_h);
                        float sa_sample = 1.f / (prefilter_sample_count * pdf + 0.0001f);

                        float mip_level = roughness == 0.f ? 0.f : 0.5f * std::log2(sa_sample / sa_texel);

                        prefilter_color += LinearSampler::sample2D(env_mipmap,uv.x,uv.y,mip_level);
                        total_weight    += NdotL;
                    }
                }
                prefilter_color /= total_weight;
                ibl.prefilter_map.get_level(i).at(w,h) = prefilter_color;
            }
        });
    }
    LOG_INFO("finish generate prefilter map");

    int brdf_lut_w = IBL::BRDFLUTSize;
    int brdf_lut_h = IBL::BRDFLUTSize;
    ibl.brdf_lut = Image<float2>(brdf_lut_w,brdf_lut_h);
    int brdf_sample_count = IBL::BRDFSampleCount;
    parallel_forrange(0,brdf_lut_h,[&](int,int h){
        for(int w = 0; w < brdf_lut_w; ++w){
            float NdotV     = (w + 0.5f) / brdf_lut_w;
            float roughness = (h + 0.5f) / brdf_lut_h;

            float A = 0.f;
            float B = 0.f;

            float3 V = float3(sqrt(1.f - NdotV * NdotV),0.f,NdotV);
            float3 N = float3(0.f,0.f,1.f);
            for(int i = 0; i < brdf_sample_count; ++i){
                float2 Xi = Hammersley(i,brdf_sample_count);
                float3 H  = ImportanceSampleGGX(Xi,N,roughness);
                float3 L  = normalize(2.f * dot(V,H) * H - V);

                float NdotL = std::max(L.z,0.f);
                float NdotH = std::max(H.z,0.f);
                float VdotH = std::max(dot(V,H),0.f);
                if(NdotL > 0.f){
                    float G     = GeometrySmith(N,V,L,roughness);
                    float G_Vis = (G*VdotH) / (NdotH * NdotV);
                    float Fc    = std::pow(1.f - VdotH, 5.f);

                    A += (1.f - Fc) * G_Vis;
                    B += Fc * G_Vis;
                }
            }
            A /= brdf_sample_count;
            B /= brdf_sample_count;
            ibl.brdf_lut.at(w,h) = {A,B};
        }
    });
    LOG_INFO("finish generate brdf lut");
}

const IBL &Model::getIBL() const
{
    return ibl;
}
