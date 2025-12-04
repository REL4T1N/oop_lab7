#pragma once

#include "../core/Point.h"
#include <memory>
#include <string>

class Visitor; // предватерительно

class NPC {
protected:
    std::string id;
    Point position;
    bool alive;
    int move_range;
    int attack_range;
    char symbol; // symbol == type используется в мапе

public:
    NPC(const std::string& name, const Point& p, int moveRange, int attackRange, char sym);
    virtual ~NPC() = default;

    const std::string& getID() const;
    const Point& getPos() const;
    void setPos(const Point& newPos);
    void markDead();
    int getMoveRange() const;
    int getAttackRange() const;
    char getSymbol() const;
    bool isAlive() const;

    virtual std::string getType() const = 0;
    virtual bool canAttack(const std::shared_ptr<NPC>& target) const = 0;
    virtual void accept(Visitor &visitor) = 0;

    double distanceTo(const std::shared_ptr<NPC>& other) const;
    bool isInAttackRange(const std::shared_ptr<NPC>& target) const;
    virtual Point calculateNextPosition() const;

    // удобная запись в файл
    virtual void save(std::ostream& os) const;
};