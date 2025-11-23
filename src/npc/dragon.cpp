#include "../../include/npc/dragon.h"
#include "../../include/visitor/visitor.h"

Dragon::Dragon(const std::string& name, const Point& pos, int range) : INPC(name, pos, range) {}

std::string Dragon::getType() const {return "Дракон";}

void Dragon::attack(INPC* target) {
    std::cout << name << " (дракон) атакует " << target->getName() << std::endl; 
}

bool Dragon::canAttack(INPC *target) const {
    return isAlive() && target->isAlive() && target->getType() == "Эльф" && distanceTo(target) <= getAttackRange();
}

void Dragon::accept(Visitor& visitor) {
    visitor.visit(*this);
}