#include "../../include/npc/druid.h"
#include "../../include/visitor/visitor.h"

Druid::Druid(const std::string& name, const Point& pos, int range) : INPC(name, pos, range) {}

std::string Druid::getType() const {return "Друид";}

void Druid::attack(INPC* target) {
    std::cout << name << " (друид) атакует " << target->getName() << std::endl; 
}

bool Druid::canAttack(INPC *target) const {
    return isAlive() && target->isAlive() && target->getType() == "Дракон" && distanceTo(target) <= getAttackRange();
}

void Druid::accept(Visitor& visitor) {
    visitor.visit(*this);
}