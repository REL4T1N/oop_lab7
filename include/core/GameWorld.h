#pragma once

#include "../npc/NPC.h"
#include <vector>
#include <shared_mutex>
#include <algorithm>
#include <fstream>

class GameWorld {
private:
    std::vector<std::shared_ptr<NPC>> npcs;
    mutable std::shared_mutex mutex;

public:
    GameWorld() = default;
    
    void addNPC(std::shared_ptr<NPC> npc);
    std::vector<std::shared_ptr<NPC>> getNPCsInRange(std::shared_ptr<NPC> source, double range);
    std::vector<std::shared_ptr<NPC>> getAllAliveNPCs() const;
    std::shared_ptr<NPC> getNPCByID(const std::string& id) const;
    
    void cleanupDeadNPCs();

    struct GameStats {
        int totalNPCs;
        int aliveNPCs;
        int orcsCount;
        int squirrelCount;
        int bearCount;
    };

    GameStats getStats() const;
    void saveToFile(const std::string& filename) const;
    void loadFromFile(const std::string& filename);
};