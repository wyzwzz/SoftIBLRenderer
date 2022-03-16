#pragma once
#include "mesh.hpp"
#include "shader.hpp"
#include "zbuffer.hpp"
class Rasterizer
{
  public:
    static bool rasterTriangle(Triangle &triangle, IShader &shader, Image<color4b> &pixels, ZBuffer &zBuffer);
    static void triangleBoundBox(const Triangle &triangle, int &xMin, int &yMin, int &xMax, int &yMax, int w, int h);
    static std::tuple<float, float, float> computeBarycentric2D(float x, float y, const Triangle &triangle);
    static bool insideTriangle(float alpha, float beta, float gamma);
    static void viewportTransform(Triangle &triangle, int w, int h);
    static void gammaAdjust(color4b &pixelColor);

  private:
    static const uint8_t gammaTable[256];
};