#include <stdexcept>

#include "displayer.hpp"
#include "logger.hpp"

Displayer::Displayer()
{
    initSDL();
}

Displayer::~Displayer()
{
    destroySDL();
}

void Displayer::draw(const Image<color4b> &pixels)
{
    static SDL_Rect rect{0, 0, ScreenWidth, ScreenHeight};
    SDL_UpdateTexture(texture, nullptr, pixels.data(), pixels.pitch());
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, &rect);
    SDL_RenderPresent(renderer);
}

void Displayer::initSDL()
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0)
    {
        LOG_CRITICAL("{} - SDL could not initialize! SDL Error: {}",__FUNCTION__ ,SDL_GetError());
        throw std::runtime_error("SDL could not initialize");
    }

    window = SDL_CreateWindow("SoftPBRRenderer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              ScreenWidth,ScreenHeight, 0);
    if (!window)
    {
        LOG_CRITICAL("{} - SDL could not create window! SDL Error: {}",__FUNCTION__ ,SDL_GetError());
        throw std::runtime_error("SDL create window failed");
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer)
    {
        LOG_CRITICAL("{} - SDL could not create SDL_Renderer! SDL Error: {}\n",__FUNCTION__ ,SDL_GetError());
        throw std::runtime_error("SDL create window surface failed");
    }
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING,
                                ScreenWidth, ScreenHeight);
}

void Displayer::destroySDL()
{
    SDL_DestroyWindow(window);
    SDL_Quit();
}
