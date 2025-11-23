#pragma once

#include "./NPC.h"

class Druid : public INPC {
public:
    Druid(const std::string& name, const Point& pos, int range);

    std::string getType() const override;
    void attack(INPC* target) override;
    bool canAttack(INPC* target) const override;
    
    void accept(Visitor& visitor) override;
};