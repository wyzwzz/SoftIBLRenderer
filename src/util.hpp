//
// Created by wyz on 2022/2/24.
//
#pragma once

#include <chrono>
#include <iostream>
#define START_TIMER auto __start = std::chrono::steady_clock::now();

#define STOP_TIMER(msg) \
auto __end = std::chrono::steady_clock::now();\
auto __t = std::chrono::duration_cast<std::chrono::milliseconds>(__end-__start);\
std::cout<<msg<<" cost time: "<<__t.count()<<"ms"<<std::endl;
