//
// Created by wyz on 2022/2/15.
//
#pragma once
#include <stdexcept>
template<typename T>
class Image{
    int w,h;
    T* d;
    int size;
    struct UnInit{};
    Image(int w,int h,UnInit):w(w),h(h),size(w*h){
        d = static_cast<T*>(::operator new(sizeof(T)*size));
    }
  public:
    using Self = Image<T>;
    Image():w(0),h(0),d(nullptr),size(0){}
    Image(int w, int h, const T& init_value = T{}):Image(w,h,UnInit{}){
        for(int i =0;i<size;i++)
            new (d+i)T(init_value);
    }
    Image(const Self& other):w(other.w),h(other.h),size(other.size){
        if(other.isAvailable()){
            d = static_cast<T*>(::operator new(sizeof(T)*size));
            for(int i =0;i<size;i++){
                new (d + i)T(other.d[i]);
            }
        }
        else{
            this->destroy();
        }
    }
    Image(Image&& rhs) noexcept
    :w(rhs.w),h(rhs.h),size(rhs.size),d(rhs.d)
    {
        rhs.w = rhs.h = rhs.size = 0;
        rhs.d = nullptr;
    }
    Self& operator=(const Self& other){
        this->destroy();
        new (this)Self(other);
        return *this;
    }

    Self& operator=(Self&& rhs) noexcept{
        this->destroy();
        new(this) Self(std::move(rhs));
        return *this;
    }


    T& operator()(int x, int y){
        return d[toLinearIndex(x,y)];
    }

    const T& operator()(int x, int y) const {
        return d[toLinearIndex(x,y)];
    }

    T& at(int x, int y){
        if(x>=w || x<0 || y>=h || y<0)
            throw std::out_of_range("Image at out of range");
        return d[toLinearIndex(x,y)];
    }

    int toLinearIndex(int x, int y) const{
        return y * w + x;
    }

    bool isAvailable() const
    {
        return d != nullptr;
    };
    void destroy(){
        if(isAvailable()){
            ::operator delete(d);
            d = nullptr;
        }
        w = h = size = 0;
    }

    int width() const { return w; }
    int height() const { return h; }
    int pitch() const { return sizeof(T) * w; }
    const T* data() const { return d; }
    T* data() { return d; }
};
