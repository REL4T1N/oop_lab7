#pragma once

#include <memory>
#include <unordered_map>
#include "./NPCCreator.h"
#include <vector>
#include "./elfCreator.h"
#include "./dragonCreator.h"
#include "./druidCreator.h"


class NPCFactory {
private:
    std::unordered_map<std::string, std::unique_ptr<INPCCreator>> creators;

public:
    NPCFactory();

    std::unique_ptr<INPC> createNPC(const std::string& name, const std::string& type, int x, int y, int range);

    std::vector<std::unique_ptr<INPC>> loadFromFile(const std::string& name);
    void saveToFile(const std::string& filename, const std::vector<std::unique_ptr<INPC>>& npcs);
};
