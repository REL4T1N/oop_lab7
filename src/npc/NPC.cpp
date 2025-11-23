#include "../../include/npc/NPC.h"

INPC::INPC(const std::string& name, const Point& pos, int range) : name(name), position(pos), alive(true), attackRange(range) {}

std::string INPC::getName() const {return name;}

const Point& INPC::getPosition() const {return position;}

bool INPC::isAlive() const {return alive;}

void INPC::markDead() {alive = false;}

int INPC::getAttackRange() const {return attackRange;}

void INPC::setAttackRange(int range) {attackRange = range;}

double INPC::distanceTo(const INPC* other) const {
    return position.distanceTo(other->position);
}

bool INPC::canAttack(INPC* target) const {
    return isAlive() && target->isAlive() && distanceTo(target) <= attackRange;
}