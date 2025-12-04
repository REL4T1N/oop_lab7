#include "../../include/factory/BearCreator.h"

std::shared_ptr<NPC> BearCreator::createNPC(const std::string &id) {
    return std::make_shared<Bear>(id, generateRandomPosition());
}

std::shared_ptr<NPC> BearCreator::createNPC(const std::string &id, const Point &p) {
    return std::make_shared<Bear>(id, p);
}

std::shared_ptr<NPC> BearCreator::createNPC(const std::string &id, const Point &p, int moveRange, int attackRange) {
    return std::make_shared<Bear>(id, p);
}