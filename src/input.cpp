#include "input.hpp"
#include <iostream>
InputProcessor::InputProcessor(const std::shared_ptr<Scene> &scene) : scene(scene)
{
}

InputProcessor::~InputProcessor()
{
}
void InputProcessor::processInput(bool &exit, uint32_t delta_t)
{
    static SDL_Event event;
    auto camera = scene->getCamera();
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT: {
            exit = true;
            break;
        }
        case SDL_DROPFILE: {
            std::cout << "load scene file: " << event.drop.file << std::endl;
            try
            {
                scene->clearScene();
                scene->loadScene(std::string(event.drop.file));
            }
            catch (const std::exception &err)
            {
                std::cout << err.what() << std::endl;
            }
            break;
        }
        case SDL_KEYDOWN: {
            switch (event.key.keysym.sym)
            {
            case SDLK_w: {
                camera->position += camera->front * camera->move_speed;
                break;
            }
            case SDLK_s: {
                camera->position -= camera->front * camera->move_speed;
                break;
            }
            case SDLK_d: {
                camera->position += camera->right * camera->move_speed;
                break;
            }
            case SDLK_a: {
                camera->position -= camera->right * camera->move_speed;
                break;
            }
            case SDLK_q: {
                camera->position += camera->up * camera->move_speed;
                break;
            }
            case SDLK_e: {
                camera->position -= camera->up * camera->move_speed;
                break;
            }
            }
            camera->target = camera->position + camera->front;
            break;
        }
        case SDL_MOUSEMOTION: {

            if (event.motion.state & SDL_BUTTON_LMASK)
            {
                float x_offset = static_cast<float>(event.motion.xrel) * camera->move_sense;
                float y_offset = static_cast<float>(event.motion.yrel) * camera->move_sense;

                camera->yaw += x_offset;
                camera->pitch -= y_offset;

                if (camera->pitch > 89.f)
                {
                    camera->pitch = 89.f;
                }
                else if (camera->pitch < -89.f)
                {
                    camera->pitch = -89.f;
                }

                camera->front.x = std::cos(camera->pitch * M_PI / 180.f) * std::cos(camera->yaw * M_PI / 180.f);
                camera->front.y = std::sin(camera->pitch * M_PI / 180.f);
                camera->front.z = std::cos(camera->pitch * M_PI / 180.f) * std::sin(camera->yaw * M_PI / 180.f);
                camera->front = normalize(camera->front);
                camera->right = normalize(cross(camera->front, camera->world_up));
                camera->up = normalize(cross(camera->right, camera->front));
                camera->target = camera->position + camera->front;
            }
            break;
        }
        case SDL_MOUSEWHEEL: {
            static float zoom = 2.f;
            if (event.wheel.y > 0)
            {
                camera->fov -= zoom;
            }
            else if (event.wheel.y < 0)
            {
                camera->fov += zoom;
            }
            if (camera->fov < 12.f)
            {
                camera->fov = 12.f;
            }
            else if (camera->fov > 120.f)
            {
                camera->fov = 120.f;
            }
            break;
        }
        }
    }
}
