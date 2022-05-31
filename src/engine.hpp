#pragma once

#include <string>

#include "common.hpp"

class Engine final
{
    Engine() = default;

  public:

    ~Engine() = default;

    Engine(const Engine&) = delete;

    Engine& operator=(const Engine&) = delete;

    static Engine& getInstance();

    void startup();

    void run();

    void shutdown();

  private:

    RC<Scene> scene;

    Box<Displayer> displayer;

    Box<SoftRenderer> soft_renderer;

    Box<InputProcessor> input_processor;
};
