//
// Created by User on 11/10/2026.
//
#include "Timer.h"

std::string Timer::indent() { return std::string(depth * 2, '  '); }

Timer::Timer(const std::string_view timerName) : timerName{timerName},
                                                       startTime{std::chrono::steady_clock::now()} {

    std::cout << indent() << "> " << timerName << '\n';
    ++depth;
}

Timer::~Timer(){
    --depth;

    const auto endTime{std::chrono::steady_clock::now()};
    std::cout << indent() << "- " << timerName << " duration : " << std::chrono::duration_cast<
        std::chrono::milliseconds>(endTime - startTime) << '\n';
}
