//
// Created by wyz on 2022/2/15.
//
#pragma once
#include <SDL.h>
#include "scene.hpp"
#include <memory>
/**
 * @brief process mouse, keyboard and file drag events.
 */
class InputProcessor{
  public:

    explicit InputProcessor(const std::shared_ptr<Scene>& scene);

    ~InputProcessor();

    void processInput(bool& exit,uint32_t delta_t);

  private:
    std::shared_ptr<Scene> scene;
};
