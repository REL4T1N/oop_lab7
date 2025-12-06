#pragma once
#include <chrono>

namespace GameConfig {
    constexpr int MAP_WIDTH = 25; // 100;
    constexpr int MAP_HEIGHT = 25; // 100;
    constexpr auto GAME_DURATION = std::chrono::seconds(10);
    constexpr auto RENDER_INTERVAL = std::chrono::seconds(1);
    constexpr auto NPC_UPDATE_INTERVAL = std::chrono::milliseconds(500);
    constexpr int INITIAL_NPC_COUNT = 25; // 50;
    
    namespace Orc {
        constexpr int MOVE_RANGE = 20;
        constexpr int ATTACK_RANGE = 10;
        constexpr char SYMBOL = 'O';
    }
    
    namespace Squirrel {
        constexpr int MOVE_RANGE = 5;
        constexpr int ATTACK_RANGE = 5;
        constexpr char SYMBOL = 'S';
    }
    
    namespace Bear {
        constexpr int MOVE_RANGE = 5;
        constexpr int ATTACK_RANGE = 10;
        constexpr char SYMBOL = 'B';
    }
    
    constexpr int DICE_SIDES = 6;
}