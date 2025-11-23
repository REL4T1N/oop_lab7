#include "../../include/factory/dragonCreator.h"


std::unique_ptr<INPC> DragonCreator::createNPC(const std::string& name, int x, int y, int range) {
    Point pos(x, y);
    return std::make_unique<Dragon>(name, pos, range);
};