#pragma once
#include "buffer.hpp"
#include "geometry.hpp"
#include <memory>
class ZBuffer
{
  public:
    ZBuffer() = default;
    virtual bool zTest(int x, int y, float zVal) const = 0;
    virtual bool zTest(const BoundBox2D &box, float minZVal) const
    {
        return true;
    };
    virtual void updateZBuffer(int x, int y, float zVal) = 0;
    virtual void clear() = 0;
    virtual ~ZBuffer()
    {
    }
};
class NaiveZBuffer : public ZBuffer
{
  public:
    NaiveZBuffer(int w, int h);
    bool zTest(const BoundBox2D &box, float minZVal) const override
    {
        return true;
    };
    bool zTest(int x, int y, float zVal) const override;
    void updateZBuffer(int x, int y, float zVal) override;
    void clear() override;

  private:
    Image<float> z_buffer;
};

class HierarchicalZBuffer : public ZBuffer
{
  public:
    HierarchicalZBuffer(int w, int h);
    bool zTest(const BoundBox2D &box, float minZVal) const override;
    bool zTest(int x, int y, float zVal) const override;
    void updateZBuffer(int x, int y, float zVal) override;
    void clear() override;
    ~HierarchicalZBuffer() override;

  private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};
