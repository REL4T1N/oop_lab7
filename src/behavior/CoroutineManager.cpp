#include "../../include/behavior/CoroutineManager.h"
#include "../../include/behavior/NPCBehavior.h"
#include "../../include/behavior/Dice.h"
#include "../../include/core/GameWorld.h"
#include "../../include/observer/BattleLogger.h"

CoroutineManager::CoroutineManager(std::shared_ptr<GameWorld> world, std::shared_ptr<Dice> dice, std::shared_ptr<BattleLogger> logger) : world(world), dice(dice), logger(logger) {}

CoroutineManager::~CoroutineManager() {stop();}

void CoroutineManager::run() {
    TS_PRINTLN("[COROUTINE] Менеджер корутин запущен");

    while (running) {
        std::this_thread::sleep_for(GameConfig::NPC_UPDATE_INTERVAL);
        std::lock_guard<std::mutex> lock(mutex);

        for (auto& coro : coroutines) {
            if (coro.npc->isAlive()) {
                coro.behavior.next();
            }
        }
        if (!running) break;
    }
    TS_PRINTLN("[COROUTINE] Менеджер корутин остановлен");
}


BehaviorGenerator<bool> CoroutineManager::createNPCBehavior(std::shared_ptr<NPC> npc) {
    return ::createNPCBehavior(npc, world, dice, logger);
}

void CoroutineManager::addNPC(std::shared_ptr<NPC> npc) {
    std::lock_guard<std::mutex> lock(mutex);
    auto behavior = createNPCBehavior(npc);
    coroutines.emplace_back(npc, std::move(behavior));
}

void CoroutineManager::start() {
    if (running) return;

    running = true;
    coroutine_thread = std::thread([this]() {run();});
}

void CoroutineManager::stop() {
    running = false;
    if (coroutine_thread.joinable()) {
        coroutine_thread.join();
    }
}

std::vector<std::shared_ptr<NPC>> CoroutineManager::getAliveNPCs() const {
    std::lock_guard<std::mutex> lock(mutex);
    std::vector<std::shared_ptr<NPC>> alive;
    for (const auto& coro : coroutines) {
        if (coro.npc->isAlive()) {
            alive.push_back(coro.npc);
        }
    }
    return alive;
}


