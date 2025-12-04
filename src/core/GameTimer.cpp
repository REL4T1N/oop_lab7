#include "../../include/core/GameTimer.h"
#include <iostream>

void GameTimer::start() {
    start_time = std::chrono::steady_clock::now();
    game_running = true; game_finished = false;
}

void GameTimer::stop() {
    game_running = false; game_finished = true;
}

bool GameTimer::shouldContinue() const {
    if (!game_running || game_finished) {
        return false;
    }
    
    auto elapsed = getElapsedTime();
    auto remaining = GameConfig::GAME_DURATION - elapsed; 
    return elapsed < GameConfig::GAME_DURATION;
}

std::chrono::seconds GameTimer::getElapsedTime() const {
    if (!game_running) {
        return std::chrono::seconds(0);
    }
    
    auto elapsed = std::chrono::steady_clock::now() - start_time;
    return std::chrono::duration_cast<std::chrono::seconds>(elapsed);
}

std::chrono::seconds GameTimer::getRemainingTime() const {
    auto remaining = GameConfig::GAME_DURATION - getElapsedTime();
    return std::chrono::duration_cast<std::chrono::seconds>(std::max(remaining, std::chrono::seconds(0)));
}

bool GameTimer::isRunning() const {return game_running;}
bool GameTimer::isFinished() const {return game_finished;}