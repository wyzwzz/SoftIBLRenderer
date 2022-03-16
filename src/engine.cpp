#include "engine.hpp"
#include "displayer.hpp"
#include "input.hpp"
#include "renderer.hpp"
#include "util.hpp"
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
    while (!exit)
    {

        last_t = SDL_GetTicks();

        input_processor->processInput(exit, delta_t);

        START_TIMER
        soft_renderer->clearFrameBuffer();
        STOP_TIMER("clear framebuffer")

        START_TIMER
        soft_renderer->render();
        STOP_TIMER("render a frame")

        START_TIMER
        displayer->draw(soft_renderer->getImage());
        STOP_TIMER("draw frame")

        delta_t = SDL_GetTicks() - last_t;
    }
}
