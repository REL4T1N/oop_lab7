#include "../../include/factory/druidCreator.h"


std::unique_ptr<INPC> DruidCreator::createNPC(const std::string& name, int x, int y, int range) {
    Point pos(x, y);
    return std::make_unique<Druid>(name, pos, range);
};