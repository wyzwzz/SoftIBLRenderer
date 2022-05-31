#pragma once

#include <SDL.h>

#include "scene.hpp"

/**
 * @brief process mouse, keyboard and file drag events.
 */
class InputProcessor
{
  public:
    explicit InputProcessor(const std::shared_ptr<Scene> &scene);

    ~InputProcessor();

    void processInput(bool &exit, uint32_t delta_t);

  private:
    RC<Scene> scene;
};
