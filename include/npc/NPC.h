#pragma once

#include <iostream>
#include <string>
#include "../geom/point.h"

class Visitor;

class INPC {
protected:
    std::string name;
    Point position;
    bool alive;
    int attackRange;

public:
    INPC(const std::string& name, const Point& pos, int range);
    virtual ~INPC() = default;

    virtual std::string getType() const = 0;
    virtual void attack(INPC* target) = 0;
    virtual bool canAttack(INPC* target) const = 0;

    std::string getName() const;
    const Point& getPosition() const;
    bool isAlive() const;
    void markDead();
    int getAttackRange() const;  
    void setAttackRange(int range); 

    double distanceTo(const INPC* other) const;

    virtual void accept(Visitor& visitor) = 0;
};