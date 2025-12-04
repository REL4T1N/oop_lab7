#pragma once

#include "./NPC.h"
#include "../visitor/Visitor.h"

class Orc : public NPC {
public:
    // просто конструктор
    Orc(const std::string& id, const Point& p);
    // конструктор загрузки из файла
    Orc(const std::string& id, const Point& p, int moveRange, int attackRange);

    std::string getType() const override;
    bool canAttack(const std::shared_ptr<NPC>& target) const override;
    void accept(Visitor& visitor) override;
};