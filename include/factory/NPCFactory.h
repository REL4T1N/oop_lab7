#pragma once

#include "./NPCCreator.h"
#include "./OrcCreator.h"
#include "./BearCreator.h"
#include "./SquirrelCreator.h"
#include "../core/ThreadSafeConsole.h"

#include <unordered_map>
#include <fstream>
#include <vector>

class NPCFactory {
private:
    std::unordered_map<std::string, std::unique_ptr<NPCCreator>> creators;
    int NPCCounter = 0;

    std::string generateNPCID(const std::string& type);

public:
    NPCFactory();
    
    std::shared_ptr<NPC> createRandomNPC();
    std::shared_ptr<NPC> createNPC(const std::string& type);
    std::shared_ptr<NPC> createNPC(const std::string& type, const Point& p);

    std::vector<std::shared_ptr<NPC>> loadFromFile(const std::string& filename);
    void saveToFile(const std::string& filename, const std::vector<std::shared_ptr<NPC>>& npcs);
};