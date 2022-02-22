//
// Created by wyz on 2022/2/15.
//
#include "rasterizer.hpp"
#include <algorithm>
#include <iostream>
const uint8_t Rasterizer::gammaTable[256] = {0, 21, 28, 34, 39, 43, 46,
        50, 53, 56, 59, 61, 64, 66, 68, 70, 72, 74, 76, 78, 80, 82, 84,
        85, 87, 89, 90, 92, 93, 95, 96, 98, 99, 101, 102, 103, 105, 106,
        107, 109, 110, 111, 112, 114, 115, 116, 117, 118, 119, 120, 122,
        123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135,
        136, 137, 138, 139, 140, 141, 142, 143, 144, 144, 145, 146, 147,
        148, 149, 150, 151, 151, 152, 153, 154, 155, 156, 156, 157, 158,
        159, 160, 160, 161, 162, 163, 164, 164, 165, 166, 167, 167, 168,
        169, 170, 170, 171, 172, 173, 173, 174, 175, 175, 176, 177, 178,
        178, 179, 180, 180, 181, 182, 182, 183, 184, 184, 185, 186, 186,
        187, 188, 188, 189, 190, 190, 191, 192, 192, 193, 194, 194, 195,
        195, 196, 197, 197, 198, 199, 199, 200, 200, 201, 202, 202, 203,
        203, 204, 205, 205, 206, 206, 207, 207, 208, 209, 209, 210, 210,
        211, 212, 212, 213, 213, 214, 214, 215, 215, 216, 217, 217, 218,
        218, 219, 219, 220, 220, 221, 221, 222, 223, 223, 224, 224, 225,
        225, 226, 226, 227, 227, 228, 228, 229, 229, 230, 230, 231, 231,
        232, 232, 233, 233, 234, 234, 235, 235, 236, 236, 237, 237, 238,
        238, 239, 239, 240, 240, 241, 241, 242, 242, 243, 243, 244, 244,
        245, 245, 246, 246, 247, 247, 248, 248, 249, 249, 249, 250, 250,
        251, 251, 252, 252, 253, 253, 254, 254, 255, 255
};
template <typename T>
inline auto interpolate(float alpha,float beta,float gamma,const T& v1,const T& v2,const T& v3){
    return (alpha * v1 + beta * v2 + gamma * v3) / (alpha + beta + gamma);
}

template <typename T>
inline auto interpolate(float alpha,float beta,float gamma,const T& v1,const T& v2,const T& v3,float inv_weight){
    return (alpha * v1 + beta * v2 + gamma * v3) * inv_weight;
}

void Rasterizer::rasterTriangle(Triangle &triangle, IShader &shader, Image<color4b> &pixels, ZBuffer &zBuffer)
{
    const auto& v= triangle.vertices;
    float cc1 = v[0].gl_Position.x * (v[1].gl_Position.y - v[2].gl_Position.y) + v[0].gl_Position.y * (v[2].gl_Position.x - v[1].gl_Position.x) + v[1].gl_Position.x * v[2].gl_Position.y - v[2].gl_Position.x * v[1].gl_Position.y;
    float cc2 = v[1].gl_Position.x * (v[2].gl_Position.y - v[0].gl_Position.y) + v[1].gl_Position.y * (v[0].gl_Position.x - v[2].gl_Position.x) + v[2].gl_Position.x * v[0].gl_Position.y - v[0].gl_Position.x * v[2].gl_Position.y;
    float cc3 = v[2].gl_Position.x * (v[0].gl_Position.y - v[1].gl_Position.y) + v[2].gl_Position.y * (v[1].gl_Position.x - v[0].gl_Position.x) + v[0].gl_Position.x * v[1].gl_Position.y - v[1].gl_Position.x * v[0].gl_Position.y;

    //[-1,1] -> [0.5,w-0.5]
    viewportTransform(triangle,pixels.width(),pixels.height());

    int min_x,min_y,max_x,max_y;
    triangleBoundBox(triangle,min_x,min_y,max_x,max_y,pixels.width(),pixels.height());

    for(int r = min_y; r <= max_y; r++){
        for(int c= min_x; c <= max_x; c++){
            auto [alpha,beta,gamma] = computeBarycentric2D(c+0.5f,r+0.5f,triangle);
            if(!insideTriangle(alpha,beta,gamma)) continue;
            alpha /= cc1;beta /= cc2;gamma /= cc3;
            auto inv_weight = 1.f / (alpha + beta + gamma);
            float frag_z = interpolate(alpha,beta,gamma,v[0].gl_Position.z,v[1].gl_Position.z,v[2].gl_Position.z,inv_weight);
            if(zBuffer.zTest(c,r,frag_z)){//todo z-buffer test
                auto frag_pos = interpolate(alpha,beta,gamma,v[0].pos,v[1].pos,v[2].pos,inv_weight);
                auto frag_normal = interpolate(alpha,beta,gamma,v[0].normal,v[1].normal,v[2].normal,inv_weight);
                auto frag_texcoord = interpolate(alpha,beta,gamma,v[0].tex_coord,v[1].tex_coord,v[2].tex_coord,inv_weight);
                auto pixel_color = shader.fragmentShader(frag_pos,frag_normal,frag_texcoord);
                //todo gamma correction use gamma table
//                gammaAdjust(pixel_color);
                pixels(c,r) = pixel_color;
                //todo update z-buffer
                zBuffer.updateZBuffer(c,r,frag_z);
            }
        }
    }
}
void Rasterizer::triangleBoundBox(const Triangle &triangle, int &xMin, int &yMin, int &xMax, int &yMax,int w,int h)
{
    xMax = std::max({triangle.vertices[0].gl_Position.x,triangle.vertices[1].gl_Position.x,triangle.vertices[2].gl_Position.x});
    xMin = std::min({triangle.vertices[0].gl_Position.x,triangle.vertices[1].gl_Position.x,triangle.vertices[2].gl_Position.x});

    yMax = std::max({triangle.vertices[0].gl_Position.y,triangle.vertices[1].gl_Position.y,triangle.vertices[2].gl_Position.y});
    yMin = std::min({triangle.vertices[0].gl_Position.y,triangle.vertices[1].gl_Position.y,triangle.vertices[2].gl_Position.y});

    xMax = std::min(xMax,w-1);
    xMin = std::max(xMin,0);

    yMax = std::min(yMax,h-1);
    yMin = std::max(yMin,0);
}



std::tuple<float, float, float> Rasterizer::computeBarycentric2D(float x, float y, const Triangle &triangle)
{
    const auto& v = triangle.vertices;
    float c1 = (x * (v[1].gl_Position.y - v[2].gl_Position.y) + y * (v[2].gl_Position.x - v[1].gl_Position.x) + v[1].gl_Position.x * v[2].gl_Position.y - v[2].gl_Position.x * v[1].gl_Position.y) / v[0].gl_Position.w;
    float c2 = (x * (v[2].gl_Position.y - v[0].gl_Position.y) + y * (v[0].gl_Position.x - v[2].gl_Position.x) + v[2].gl_Position.x * v[0].gl_Position.y - v[0].gl_Position.x * v[2].gl_Position.y) / v[1].gl_Position.w;
    float c3 = (x * (v[0].gl_Position.y - v[1].gl_Position.y) + y * (v[1].gl_Position.x - v[0].gl_Position.x) + v[0].gl_Position.x * v[1].gl_Position.y - v[1].gl_Position.x * v[0].gl_Position.y) / v[2].gl_Position.w;
    return {c1,c2,c3};
}
bool Rasterizer::insideTriangle(float alpha,float beta,float gamma)
{
    return (alpha>=0 && beta>=0 && gamma>=0) || (alpha<0 && beta<0 && gamma<0);
}

void Rasterizer::viewportTransform(Triangle &triangle,int w, int h)
{
    for(auto& vertex : triangle.vertices){
        vertex.gl_Position.x = (vertex.gl_Position.x + 1.f) * static_cast<float>(w) * 0.5f + 0.5f;
        vertex.gl_Position.y = (vertex.gl_Position.y + 1.f) * static_cast<float>(h) * 0.5f + 0.5f;
    }
}
void Rasterizer::gammaAdjust(color4b &pixelColor)
{
    pixelColor.r = gammaTable[pixelColor.r];
    pixelColor.g = gammaTable[pixelColor.g];
    pixelColor.b = gammaTable[pixelColor.b];
}
