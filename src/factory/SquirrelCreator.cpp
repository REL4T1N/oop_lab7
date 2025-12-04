#include "../../include/factory/SquirrelCreator.h"

std::shared_ptr<NPC> SquirrelCreator::createNPC(const std::string &id) {
    return std::make_shared<Squirrel>(id, generateRandomPosition());
}

std::shared_ptr<NPC> SquirrelCreator::createNPC(const std::string &id, const Point &p) {
    return std::make_shared<Squirrel>(id, p);
}

std::shared_ptr<NPC> SquirrelCreator::createNPC(const std::string &id, const Point &p, int moveRange, int attackRange) {
    return std::make_shared<Squirrel>(id, p);
}