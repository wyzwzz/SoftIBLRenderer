#pragma once
#include "buffer.hpp"
#include "common.hpp"
#include <SDL.h>
class Displayer
{
  public:
    constexpr static float ScreenAspectRatio = static_cast<float>(ScreenWidth) / static_cast<float>(ScreenHeight);

    void draw(const Image<color4b> &pixels);

    Displayer();

    ~Displayer();

  private:
    void initSDL();
    void destroySDL();

    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
};
