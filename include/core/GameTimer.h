#pragma once

#include "./Config.h"
#include <chrono>
#include <atomic>
#include <thread>
#include <algorithm>

class GameTimer {
private:
    std::chrono::steady_clock::time_point start_time;
    std::atomic<bool> game_running{false};
    std::atomic<bool> game_finished{false};

public:
    GameTimer() = default;

    void start();
    void stop();

    bool shouldContinue() const;
    std::chrono::seconds getRemainingTime() const;  // осталось времени
    std::chrono::seconds getElapsedTime() const;    // прошло времемени

    bool isRunning() const;
    bool isFinished() const;
};