//
// Created by wyz on 2022/2/15.
//
#include "input.hpp"
#include <iostream>
InputProcessor::InputProcessor(const std::shared_ptr<Scene>& scene):scene(scene)
{

}

InputProcessor::~InputProcessor()
{

}
void InputProcessor::processInput(bool &exit, uint32_t delta_t)
{
    SDL_Event event;
    while (SDL_PollEvent(&event)){
        switch (event.type)
        {
        case SDL_QUIT:{
            exit = true;
            break;
        }
        case SDL_DROPFILE:{
            std::cout<< event.drop.file <<std::endl;
            try{
                scene->addModel(Model(std::string(event.drop.file)));
            }
            catch (const std::exception& err)
            {
                std::cout<<err.what()<<std::endl;
            }
            break;
        }
        }
    }
}
