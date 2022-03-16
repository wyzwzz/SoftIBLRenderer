#include "engine.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
int main(int argc, char **argv)
{
    try
    {
        Engine engine;

        engine.startup();

        engine.run();

        engine.shutdown();
    }
    catch (const std::exception &err)
    {
        std::cout << err.what() << std::endl;
    }
    return 0;
}