#include <iostream>

#include "engine.hpp"
#include "logger.hpp"

extern bool use_hz;

void SetArgv(int argc, char** argv){
    for(int i = 0; i < argc; ++i){
        auto arg = std::string(argv[i]);
        if(arg == "-hz"){
            use_hz = true;
        }
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
            std::cerr<<"params format: [-hz], [-debug] or [-info] or [-error]"<<std::endl;
        }
    }
}

int main(int argc, char **argv)
{
    if(argc > 1){
        SetArgv(argc - 1,argv + 1);
    }
    else{
        SET_LOG_LEVEL_CRITICAL
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