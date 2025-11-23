#include "../../include/npc/elf.h"
#include "../../include/visitor/visitor.h"

Elf::Elf(const std::string& name, const Point& pos, int range) : INPC(name, pos, range) {}

std::string Elf::getType() const {return "Эльф";}

void Elf::attack(INPC* target) {
    std::cout << name << " (эльф) атакует " << target->getName() << std::endl; 
}

bool Elf::canAttack(INPC *target) const {
    return isAlive() && target->isAlive() && target->getType() == "Друид" && distanceTo(target) <= getAttackRange();
}

void Elf::accept(Visitor& visitor) {
    visitor.visit(*this);
}