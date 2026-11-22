//
// Created by User on 11/10/2026.
//
#include "Timer.h"
#include <iostream>
std::string Timer::indent() { return std::string(depth * 2, ' '); }

Timer::Timer(const std::string_view timerName, const bool requiresDetailedMode) : timerName_{timerName},
                                                                  startTime_{std::chrono::steady_clock::now()},
                                                                  requiresDetailedMode_{requiresDetailedMode} {
    if (!detailedMode && requiresDetailedMode) return;
        std::cout << indent() << "> " << timerName << '\n';
        ++depth;

}

void Timer::setDetailedMode() {
    detailedMode = true;
}

Timer::~Timer() {
    if (!detailedMode && requiresDetailedMode_) return;

    --depth;

    const auto endTime{std::chrono::steady_clock::now()};
    std::cout << indent() << "- " << timerName_ << " duration : " << std::chrono::duration_cast<
        std::chrono::milliseconds>(endTime - startTime_) << '\n';
}
