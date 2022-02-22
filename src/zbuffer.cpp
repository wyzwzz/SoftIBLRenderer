//
// Created by wyz on 2022/2/15.
//
#include "zbuffer.hpp"
NaiveZBuffer::NaiveZBuffer(int w, int h)
{
    z_buffer = Image<float>(w,h,std::numeric_limits<float>::max());
}
bool NaiveZBuffer::zTest(int x, int y, float zVal)
{
    return zVal>=0.f && zVal<=1.f && zVal < z_buffer(x,y);
}
void NaiveZBuffer::updateZBuffer(int x, int y, float zVal)
{
    z_buffer(x,y) = zVal;
}
