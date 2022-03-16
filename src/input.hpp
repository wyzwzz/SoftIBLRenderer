#pragma once
#include "scene.hpp"
#include <SDL.h>
#include <memory>
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
    std::shared_ptr<Scene> scene;
};
