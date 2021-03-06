#pragma once

#include <algorithm>

#include "buffer.hpp"
#include "common.hpp"


template <typename T>
using Texture = Image<T>;

struct LinearSampler
{
    template <typename T>
    static auto sample2D(const Image<T> &tex, float u, float v)
    {
        u = std::clamp(u, 0.0f, 1.0f) * (tex.width() - 1);
        v = std::clamp(v, 0.0f, 1.0f) * (tex.height() - 1);
        int u0 = std::clamp(static_cast<int>(u), 0, static_cast<int>(tex.width() - 1));
        int u1 = std::clamp(u0 + 1, 0, static_cast<int>(tex.width() - 1));
        int v0 = std::clamp(static_cast<int>(v), 0, static_cast<int>(tex.height() - 1));
        int v1 = std::clamp(v0 + 1, 0, static_cast<int>(tex.height() - 1));
        float d_u = u - u0;
        float d_v = v - v0;
        return (tex(u0, v0) * (1.0f - d_u) + tex(u1, v0) * d_u) * (1.0f - d_v) +
               (tex(u0, v1) * (1.0f - d_u) + tex(u1, v1) * d_u) * d_v;
    }

    template<typename Texel>
    static auto sample2D(const MipMap2D<Texel>& mipmap,float u,float v,float level){
        assert(mipmap.valid());
        int max_level = mipmap.levels() - 1;
        level = std::clamp<float>(level,0,max_level);
        int l0 = std::floor(level);
        int l1 = l0 + 1;
        float w0 = l1 - level;
        float w1 = level - l0;
        l1 = std::clamp(l1,0,max_level);
        auto v0 = sample2D(mipmap.get_level(l0),u,v);
        auto v1 = sample2D(mipmap.get_level(l1),u,v);
        return v0 * w0 + v1 * w1;
    }
};