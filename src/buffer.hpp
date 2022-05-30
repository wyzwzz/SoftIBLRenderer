#pragma once
#include <omp.h>
#include <stdexcept>
#include <vector>
#include <cassert>
#include <iostream>
template <typename T>
class Image
{
    int w, h;
    T *d;
    int size;
    struct UnInit
    {
    };
    Image(int w, int h, UnInit) : w(w), h(h), size(w * h)
    {
        d = static_cast<T *>(::operator new(sizeof(T) * size));
    }

  public:
    using Self = Image<T>;
    Image() : w(0), h(0), d(nullptr), size(0)
    {
    }
    Image(int w, int h, const T &init_value = T{}) : Image(w, h, UnInit{})
    {
        for (int i = 0; i < size; i++)
            new (d + i) T(init_value);
    }
    Image(int w,int h,const T* p)
    :Image(w,h,UnInit{})
    {
        memcpy(d,p,sizeof(T) * w * h);
    }
    Image(const Self &other) : w(other.w), h(other.h), size(other.size)
    {
        if (other.isAvailable())
        {
            d = static_cast<T *>(::operator new(sizeof(T) * size));
            for (int i = 0; i < size; i++)
            {
                new (d + i) T(other.d[i]);
            }
        }
        else
        {
            this->destroy();
        }
    }
    Image(Image &&rhs) noexcept : w(rhs.w), h(rhs.h), size(rhs.size), d(rhs.d)
    {
        rhs.w = rhs.h = rhs.size = 0;
        rhs.d = nullptr;
    }
    Self &operator=(const Self &other)
    {
        this->destroy();
        new (this) Self(other);
        return *this;
    }

    Self &operator=(Self &&rhs) noexcept
    {
        this->destroy();
        new (this) Self(std::move(rhs));
        return *this;
    }

    T &operator()(int x, int y)
    {
        return d[toLinearIndex(x, y)];
    }

    const T &operator()(int x, int y) const
    {
        return d[toLinearIndex(x, y)];
    }

    T &at(int x, int y)
    {
        if (x >= w || x < 0 || y >= h || y < 0)
            throw std::out_of_range("Image at out of range");
        return d[toLinearIndex(x, y)];
    }

    int toLinearIndex(int x, int y) const
    {
        return y * w + x;
    }

    bool isAvailable() const
    {
        return d != nullptr;
    }
    void destroy()
    {
        if (isAvailable())
        {
            ::operator delete(d);
            d = nullptr;
        }
        w = h = size = 0;
    }

    void clear()
    {
#pragma omp parallel for
        for (int i = 0; i < w * h; i++)
        {
            d[i] = T{};
        }
    }

    int width() const
    {
        return w;
    }
    int height() const
    {
        return h;
    }
    int pitch() const
    {
        return sizeof(T) * w;
    }
    const T *data() const
    {
        return d;
    }
    T *data()
    {
        return d;
    }
};
template <class T>
using Image2D = Image<T>;

template<typename T>
class MipMap2D{
  public:
    MipMap2D(){}
    explicit MipMap2D(const Image2D<T>& lod0_image){
//        generate(lod0_image);
    }
    void generate(const Image2D<T>& lod0_image);

    int levels() const{
        return images.size();
    }
    bool valid() const{
        return levels() > 0;
    }
    const Image2D<T>& get_level(int level) const{
        assert(level >=0 && level <levels());
        return images[level];
    }
  private:
    std::vector<Image2D<T>> images;
};

template<typename T>
void MipMap2D<T>::generate(const Image2D<T> &lod0_image) {
    std::cout<<"start generate with :"<<lod0_image.width()<<" "<<lod0_image.height()<<std::endl;
    this->images.clear();
    int last_w = lod0_image.width();
    int last_h = lod0_image.height();
    images.emplace_back(lod0_image);
    while(last_w > 1 && last_h > 1){
        if((last_w & 1) || (last_h & 1)){
            this->images.clear();
            throw std::runtime_error("invalid input image size: must be power of 2");
        }
        const int cur_w = last_w >> 1;
        const int cur_h = last_h >> 1;
        Image2D<T> cur_lod_image(cur_w,cur_h);
        auto& last_lod_image = images.back();
        for(int y = 0; y < cur_h; ++y){
            const int ly = y << 1;
            for(int x = 0; x < cur_w; ++x){
                const int lx = x << 1;
                const auto t00 = last_lod_image.at(lx,ly);
                const auto t01 = last_lod_image.at(lx+1,ly);
                const auto t10 = last_lod_image.at(lx,ly+1);
                const auto t11 = last_lod_image.at(lx+1,ly+1);
                cur_lod_image.at(x,y) = (t00 + t01 + t10 + t11) * 0.25f;
            }
        }
        images.emplace_back(std::move(cur_lod_image));
        last_w >>= 1;
        last_h >>= 1;
    }
    std::cout<<"total mipmap levels "<<images.size()<<std::endl;
}

