#include "../../include/factory/NPCFactory.h"

std::string NPCFactory::generateNPCID(const std::string& type) {
    return type + "_" + std::to_string(++NPCCounter);
}

NPCFactory::NPCFactory() {
    creators["Орк"] = std::make_unique<OrcCreator>();
    creators["Медведь"] = std::make_unique<BearCreator>();
    creators["Белка"] = std::make_unique<SquirrelCreator>();
}

std::shared_ptr<NPC> NPCFactory::createRandomNPC() {
    static std::vector<std::string> types = {"Орк", "Медведь", "Белка"};
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> typeDist(0, types.size() - 1);

    std::string type = types[typeDist(gen)];
    return createNPC(type);
}

std::shared_ptr<NPC> NPCFactory::createNPC(const std::string &type) {
    auto it = creators.find(type);
    if (it == creators.end()) {
        throw std::invalid_argument("Неизвестный тип NPC: " + type);
    }

    std::string id = generateNPCID(type);
    auto npc = it->second->createNPC(id);
    TS_PRINTLN("Создан " + type + " с ID: " + id);   // эта чо
    return npc;
}

std::shared_ptr<NPC> NPCFactory::createNPC(const std::string& type, const Point& p) {
    auto it = creators.find(type);
    if (it == creators.end()) {
        throw std::invalid_argument("Неизвестный тип NPC: " + type);
    }

    std::string id = generateNPCID(type);
    return it->second->createNPC(id, p);
}

std::vector<std::shared_ptr<NPC>> NPCFactory::loadFromFile(const std::string &filename) {
    std::vector<std::shared_ptr<NPC>> npcs;
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Не удалось открыть файл: " + filename);
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string type, id;
        int x, y, moveRange, attackRange;

        if (iss >> type >> id >> x >> y >> moveRange >> attackRange) {
            try {
                Point pos(x, y);
                auto npc = createNPC(type, pos);
                npcs.emplace_back(npc);
                TS_PRINTLN("Загружен " + type + " из файла: " + id);
            }
            catch (const std::exception& e) {
                TS_PRINTLN("Ошибка загрузки NPC: " + std::string(e.what()));
            }
        }
    }
    return npcs;
}

void NPCFactory::saveToFile(const std::string &filename, const std::vector<std::shared_ptr<NPC>> &npcs) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Не удалось открыть файл: " + filename);
    }

    for (const auto& npc : npcs) {
        if (npc->isAlive()) {
            npc->save(file);
        }
    }
    TS_PRINTLN("Сохранено " + std::to_string(npcs.size()) + " NPC в файл: " + filename);
}