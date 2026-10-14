//
// Created by LeeEeZian on 14/10/2026.
//

#ifndef ZIP_BOMB_CREATOR_TIMER_H
#define ZIP_BOMB_CREATOR_TIMER_H
#include <chrono>
#include <iostream>

class Timer {
    static inline int depth{};
    static std::string indent() { return std::string(depth * 2, ' '); }
public:
    explicit Timer(const std::string_view timerName) : timerName{timerName},
                                                       startTime{std::chrono::steady_clock::now()} {

        std::cout << indent() << "> " << timerName << '\n';
        ++depth;

    }

    ~Timer() {
        --depth;

        const auto endTime{std::chrono::steady_clock::now()};
        std::cout << indent() << "> " << timerName << " duration : " << std::chrono::duration_cast<
            std::chrono::milliseconds>(endTime - startTime) << '\n';
    }

private:
    std::string timerName;
    std::chrono::steady_clock::time_point startTime;
};
#endif //ZIP_BOMB_CREATOR_TIMER_H
