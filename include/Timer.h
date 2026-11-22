//
// Created by LeeEeZian on 14/10/2026.
//

#ifndef ZIP_BOMB_CREATOR_TIMER_H
#define ZIP_BOMB_CREATOR_TIMER_H
#include <chrono>

class Timer {
    static inline int depth{};
    static std::string indent();
    static inline bool detailedMode{false};
public:
    explicit Timer( std::string_view timerName,bool requiresDetailedMode = false);
    static void setDetailedMode();
    ~Timer();
private:
    std::string timerName_;
    std::chrono::steady_clock::time_point startTime_;
    const bool requiresDetailedMode_{};
};
#endif //ZIP_BOMB_CREATOR_TIMER_H
