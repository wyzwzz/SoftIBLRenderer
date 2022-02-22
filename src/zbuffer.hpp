//
// Created by wyz on 2022/2/15.
//
#pragma once
#include "buffer.hpp"
class ZBuffer{
  public:
    ZBuffer() = default;
    virtual bool zTest(int x,int y,float zVal) = 0;
    virtual void updateZBuffer(int x,int y,float zVal) = 0;

};
class NaiveZBuffer: public ZBuffer{
  public:
    NaiveZBuffer(int w,int h);
    bool zTest(int x,int y,float zVal) override;
    void updateZBuffer(int x,int y,float zVal) override;

  private:
    Image<float> z_buffer;
};

class HierarchyZBuffer: public ZBuffer{
  public:

};
