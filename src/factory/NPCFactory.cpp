#include "../../include/factory/NPCFactory.h"
#include <iostream>
#include <sstream>
#include <fstream>

NPCFactory::NPCFactory() {
    creators["Эльф"] = std::make_unique<ElfCreator>();
    creators["Дракон"] = std::make_unique<DragonCreator>();
    creators["Друид"] = std::make_unique<DruidCreator>();
}

std::unique_ptr<INPC> NPCFactory::createNPC(const std::string& name, const std::string& type, int x, int y, int range) {
    auto it = creators.find(type);
    if (it == creators.end()) {
        throw std::invalid_argument("Неизвестный NPC: " + type);
    }
    return it->second->createNPC(name, x, y, range);
}

std::vector<std::unique_ptr<INPC>> NPCFactory::loadFromFile(const std::string& filename) {
    std::vector<std::unique_ptr<INPC>> npcs;
    std::ifstream file(filename);

    if (!file.is_open()) {
        throw std::runtime_error("Не удалось открыть файл " + filename);
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);       // iss() -> <sstream>
        std::string name, type;
        int x, y, range;

        if (iss >> name >> type >> x >> y >> range) {
            try {
                auto npc = createNPC(name, type, x, y, range);
                npcs.push_back(std::move(npc));
            } catch (const std::exception& e) {
                std::cerr << "Ошибка создания NPC по полученным из файла данным: " << e.what() << std::endl;
            }
        }
    }

    return npcs;
}


void NPCFactory::saveToFile(const std::string& filename, const std::vector<std::unique_ptr<INPC>>& npcs) {
    std::ofstream file(filename);

    if (!file.is_open()) {
        throw std::runtime_error("Не удалось открыть файл " + filename);
    }

    for (const auto& npc : npcs) {
        if (npc->isAlive()) {
            file << npc->getName() << " " << npc->getType() << " " << npc->getPosition() << " " << npc->getAttackRange() <<"\n";
        }
    }
}
