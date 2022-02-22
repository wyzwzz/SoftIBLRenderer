//
// Created by wyz on 2022/2/15.
//
#pragma once
#include <memory>
#include <string>
class Scene;
class Displayer;
class SoftRenderer;
class InputProcessor;
class Engine{
  public:
    Engine();
    ~Engine();
    void startup();
    void run();
    void shutdown();
  private:
    std::shared_ptr<Scene> scene;
    std::unique_ptr<Displayer> displayer;
    std::unique_ptr<SoftRenderer> soft_renderer;
    std::unique_ptr<InputProcessor> input_processor;
};
