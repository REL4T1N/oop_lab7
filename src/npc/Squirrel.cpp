#include "../../include/npc/Squirrel.h"

Squirrel::Squirrel(const std::string& id, const Point& p) : NPC(id, p, GameConfig::Squirrel::MOVE_RANGE, GameConfig::Squirrel::ATTACK_RANGE, GameConfig::Squirrel::SYMBOL) {}

Squirrel::Squirrel(const std::string& id, const Point& p, int moveRange, int attackRange) : NPC(id, p, moveRange, attackRange, GameConfig::Squirrel::SYMBOL) {}

std::string Squirrel::getType() const {return "Белка";}

bool Squirrel::canAttack(const std::shared_ptr<NPC>& target) const {
    return false;
}

void Squirrel::accept(Visitor& visitor) {
    visitor.visit(*this);
}