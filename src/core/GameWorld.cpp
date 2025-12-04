#include "../../include/core/GameWorld.h"
#include <mutex>
#include <shared_mutex>

void GameWorld::addNPC(std::shared_ptr<NPC> npc) {
    std::unique_lock<std::shared_mutex> lock(mutex);
    npcs.emplace_back(npc);
}

std::vector<std::shared_ptr<NPC>> GameWorld::getNPCsInRange(std::shared_ptr<NPC> source, double range) {
    std::shared_lock<std::shared_mutex> lock(mutex);
    std::vector<std::shared_ptr<NPC>> inRange;

    for (const auto& npc : npcs) {
        if (npc != source && npc->isAlive() && source->isAlive()) {
            double distance = source->distanceTo(npc);

            if (distance <= range) {
                inRange.emplace_back(npc);
            }
        }
    }
    return inRange;
}

std::vector<std::shared_ptr<NPC>> GameWorld::getAllAliveNPCs() const {
    std::shared_lock<std::shared_mutex> lock(mutex);
    std::vector<std::shared_ptr<NPC>> alive;
    std::copy_if(npcs.begin(), npcs.end(), std::back_inserter(alive), [](const auto& npc) {
        return npc->isAlive();
    });
    return alive;
}

std::shared_ptr<NPC> GameWorld::getNPCByID(const std::string &id) const {
    std::shared_lock<std::shared_mutex> lock(mutex);
    auto it =  std::find_if(npcs.begin(), npcs.end(), [&id](const auto& npc) {
        return npc->getID() == id;}
    );
    return it != npcs.end() ? *it : nullptr;
}

void GameWorld::cleanupDeadNPCs() {
    std::unique_lock<std::shared_mutex> lock(mutex);
    auto it = std::remove_if(npcs.begin(), npcs.end(), [](const auto& npc) {
        return !npc->isAlive();
    });
    npcs.erase(it, npcs.end());
}

GameWorld::GameStats GameWorld::getStats() const {
    std::shared_lock<std::shared_mutex> lock(mutex);
    GameStats stats{};
    stats.totalNPCs = npcs.size();

    for (const auto& npc : npcs) {
        if (npc->isAlive()) {
            stats.aliveNPCs++;
            std::string type = npc->getType();
            if (type == "Орк") stats.orcsCount++;
            else if (type == "Медведь") stats.bearCount++;
            else if (type == "Белка") stats.squirrelCount++;
        }
    }
    return stats;
}

void GameWorld::saveToFile(const std::string &filename) const {
    std::shared_lock<std::shared_mutex> lock(mutex);
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Не получилось открыть файл: " + filename);
    }

    for (const auto& npc : npcs) {
        if (npc->isAlive()) {
            npc->save(file);
        }
    }
}

void GameWorld::loadFromFile(const std::string &filename) {
    std::unique_lock<std::shared_mutex> lock(mutex);
}