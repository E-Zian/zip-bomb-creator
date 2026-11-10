//
// Created by LeeEeZian on 14/10/2026.
//

#ifndef ZIP_BOMB_CREATOR_TIMER_H
#define ZIP_BOMB_CREATOR_TIMER_H
#include <chrono>
#include <iostream>
#include <optional>
class Timer {
    static inline int depth{};
    static std::string indent();

public:
    explicit Timer( std::string_view timerName);

    ~Timer();
private:
    std::string timerName;
    std::chrono::steady_clock::time_point startTime;
};
#endif //ZIP_BOMB_CREATOR_TIMER_H
