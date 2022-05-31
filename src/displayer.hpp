#pragma once

#include <SDL.h>

#include "buffer.hpp"
#include "common.hpp"

class Displayer
{
  public:
    Displayer();

    ~Displayer();

    void draw(const Image<color4b> &pixels);

  private:
    void initSDL();

    void destroySDL();

    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
};
