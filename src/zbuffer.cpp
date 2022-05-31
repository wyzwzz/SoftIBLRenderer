//#include <omp.h>
#include "zbuffer.hpp"
#include "parallel.hpp"

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
    int width = z_buffer.width(),height = z_buffer.height();
    parallel_forrange(0,height,[&](int,int h){
        for(int w = 0; w < width; ++w){
            z_buffer(w,h) = std::numeric_limits<float>::max();
        }
    });

//#pragma omp parallel for
//    for (int i = 0; i < z_buffer.width(); i++)
//    {
//        for (int j = 0; j < z_buffer.height(); j++)
//        {
//            z_buffer(i, j) = std::numeric_limits<float>::max();
//        }
//    }
}
