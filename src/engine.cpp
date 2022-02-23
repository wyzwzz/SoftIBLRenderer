//
// Created by wyz on 2022/2/15.
//
#include "displayer.hpp"
#include "engine.hpp"
#include "input.hpp"
#include "renderer.hpp"

Engine::Engine()
{

}

Engine::~Engine()
{

}

void Engine::startup()
{
    scene = std::make_shared<Scene>();
    displayer = std::make_unique<Displayer>();
    soft_renderer = std::make_unique<SoftRenderer>(scene);
    input_processor = std::make_unique<InputProcessor>(scene);
}

void Engine::shutdown()
{

}

void Engine::run()
{
    bool exit = false;

    uint32_t delta_t = 0;
    uint32_t last_t = 0;
    while(!exit){
        last_t = SDL_GetTicks();

        input_processor->processInput(exit,delta_t);

        soft_renderer->clearFrameBuffer();

        soft_renderer->render();

        displayer->draw(soft_renderer->getImage());

        delta_t = SDL_GetTicks() - last_t;
    }
}
