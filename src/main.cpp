#include <iostream>

#include "engine.hpp"
#include "logger.hpp"

void SetLogger(const std::string& arg){
    if(arg == "-debug"){
        SET_LOG_LEVEL_DEBUG
    }
    else if(arg == "-info"){
        SET_LOG_LEVEL_INFO
    }
    else if(arg == "-error"){
        SET_LOG_LEVEL_ERROR
    }
    else{
        SET_LOG_LEVEL_CRITICAL
        std::cerr<<"set logger format: -debug or -info or -error"<<std::endl;
    }
}

int main(int argc, char **argv)
{
    if(argc > 1){
        SetLogger(std::string(argv[1]));
    }
    try
    {
        auto& engine = Engine::getInstance();

        engine.startup();

        engine.run();

        engine.shutdown();
    }
    catch (const std::exception &err)
    {
        std::cerr << err.what() << std::endl;
    }
    return 0;
}