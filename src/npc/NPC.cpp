#include "../../include/npc/NPC.h"
#include <random>

NPC::NPC(const std::string& id, const Point& p, int moveRange, int attackRange, char sym) : id(id), position(p), move_range(moveRange), attack_range(attackRange), symbol(sym), alive(true) {}

const std::string& NPC::getID() const {return id;}

const Point& NPC::getPos() const {return position;}

void NPC::setPos(const Point& newPos) {position = newPos;}

void NPC::markDead() {alive = false;}

int NPC::getMoveRange() const {return move_range;}

int NPC::getAttackRange() const {return attack_range;}

char NPC::getSymbol() const {return symbol;}

bool NPC::isAlive() const {return alive;}

double NPC::distanceTo(const std::shared_ptr<NPC>& other) const {
    return position.distanceTo(other->position);
}

bool NPC::isInAttackRange(const std::shared_ptr<NPC>& target) const {
    return alive && target->isAlive() && distanceTo(target) <= attack_range; 
}

Point NPC::calculateNextPosition() const {
    // Thread-safe генератор случайных чисел
    static thread_local std::mt19937 generator(std::random_device{}());
    std::uniform_int_distribution<int> dist(-move_range, move_range);
    
    int dx = dist(generator);
    int dy = dist(generator);
    
    int newX = position.getX() + dx;
    int newY = position.getY() + dy;
    
    // Ограничиваем границы
    newX = std::clamp(newX, 0, GameConfig::MAP_WIDTH - 1);
    newY = std::clamp(newY, 0, GameConfig::MAP_HEIGHT - 1);
    
    return Point(newX, newY);
}

void NPC::save(std::ostream& os) const {
    os << getType() << " " << id << " " << position << " " << move_range << " " << attack_range << std::endl;
}