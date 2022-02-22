//
// Created by wyz on 2022/2/14.
//
#pragma once
#include <SDL.h>
#include "buffer.hpp"
#include "common.hpp"
class Displayer{
  public:

    constexpr static float ScreenAspectRatio = static_cast<float>(ScreenWidth) / static_cast<float>(ScreenHeight);

    void draw(const Image<color4b>& pixels);

    Displayer();

    ~Displayer();
  private:
    void initSDL();
    void destroySDL();

    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
};
