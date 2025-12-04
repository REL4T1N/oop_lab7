#include "../../include/npc/Bear.h"

Bear::Bear(const std::string& id, const Point& p) : NPC(id, p, GameConfig::Bear::MOVE_RANGE, GameConfig::Bear::ATTACK_RANGE, GameConfig::Bear::SYMBOL) {}

Bear::Bear(const std::string& id, const Point& p, int moveRange, int attackRange) : NPC(id, p, moveRange, attackRange, GameConfig::Bear::SYMBOL) {}

std::string Bear::getType() const {return "Медведь";}

bool Bear::canAttack(const std::shared_ptr<NPC>& target) const {
    if (target->getID() == id) return false;
    std::string targetType = target->getType();
    return targetType == "Белка";
}

void Bear::accept(Visitor& visitor) {
    visitor.visit(*this);
}