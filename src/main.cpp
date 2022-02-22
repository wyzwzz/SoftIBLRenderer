//
// Created by wyz on 2022/2/14.
//
#include "engine.hpp"
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
int main(int argc, char** argv){
    try{
        Engine engine;

        engine.startup();

        engine.run();

        engine.shutdown();
    }
    catch (const std::exception& err)
    {
        std::cout<<err.what()<<std::endl;
    }
    return 0;
}