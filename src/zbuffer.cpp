#include "zbuffer.hpp"
#include <omp.h>
NaiveZBuffer::NaiveZBuffer(int w, int h)
{
    z_buffer = Image<float>(w, h, std::numeric_limits<float>::max());
}
bool NaiveZBuffer::zTest(int x, int y, float zVal) const
{
    return zVal >= 0.f && zVal <= 1.f && zVal < z_buffer(x, y);
}
void NaiveZBuffer::updateZBuffer(int x, int y, float zVal)
{
    z_buffer(x, y) = zVal;
}
void NaiveZBuffer::clear()
{
#pragma omp parallel for
    for (int i = 0; i < z_buffer.width(); i++)
    {
        for (int j = 0; j < z_buffer.height(); j++)
        {
            z_buffer(i, j) = std::numeric_limits<float>::max();
        }
    }
}
