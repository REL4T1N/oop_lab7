#pragma once

#include "./GameWorld.h"
#include "./GameTimer.h"
#include "../behavior/CoroutineManager.h"
#include "../behavior/NPCBehavior.h"
#include "../behavior/Dice.h"
#include "../factory/NPCFactory.h"
#include "../observer/BattleLogger.h"
#include "../observer/ConsoleLogger.h"
#include "../observer/FileLogger.h"
#include "../render/MapRenderer.h"

class Game {
private:
    std::shared_ptr<GameWorld> world;
    std::shared_ptr<CoroutineManager> coroutine_manager;
    std::shared_ptr<NPCFactory> factory;
    std::shared_ptr<BattleLogger> battle_logger;
    std::shared_ptr<MapRender>  renderer;
    std::shared_ptr<Dice> dice;
    std::shared_ptr<GameTimer> timer;

    std::thread coroutine_thread;
    std::thread render_thread;

    std::atomic<bool> running{false};
    std::atomic<bool> game_over{false};

    void initialize();
    void createInitialNPCs();
    void renderLoop();
    void finishGame();
    void stop();

    void printFinalStats();
    void saveGameStats();

public:
    Game();
    ~Game();
    void run();
};
