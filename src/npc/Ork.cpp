#include "../../include/npc/Orc.h"

Orc::Orc(const std::string& id, const Point& p) : NPC(id, p, GameConfig::Orc::MOVE_RANGE, GameConfig::Orc::ATTACK_RANGE, GameConfig::Orc::SYMBOL) {}

Orc::Orc(const std::string& id, const Point& p, int moveRange, int attackRange) : NPC(id, p, moveRange, attackRange, GameConfig::Orc::SYMBOL) {}

std::string Orc::getType() const {return "Орк";}

bool Orc::canAttack(const std::shared_ptr<NPC>& target) const {
    if (target->getID() == id) return false;
    std::string targetType = target->getType();
    return targetType == "Орк" || targetType == "Медведь";
}

void Orc::accept(Visitor& visitor) {
    visitor.visit(*this);
}