#include "../../include/factory/OrcCreator.h"

std::shared_ptr<NPC> OrcCreator::createNPC(const std::string &id) {
    return std::make_shared<Orc>(id, generateRandomPosition());
}

std::shared_ptr<NPC> OrcCreator::createNPC(const std::string &id, const Point &p) {
    return std::make_shared<Orc>(id, p);
}

std::shared_ptr<NPC> OrcCreator::createNPC(const std::string &id, const Point &p, int moveRange, int attackRange) {
    return std::make_shared<Orc>(id, p); 
}