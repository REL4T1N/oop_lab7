#include "../../include/factory/elfCreator.h"


std::unique_ptr<INPC> ElfCreator::createNPC(const std::string& name, int x, int y, int range) {
    Point pos(x, y);
    return std::make_unique<Elf>(name, pos, range);
};