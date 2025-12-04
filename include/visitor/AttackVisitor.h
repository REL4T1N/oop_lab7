#pragma once

#include "./Visitor.h"
#include "../npc/Orc.h"
#include "../npc/Bear.h"
#include "../npc/Squirrel.h"
#include "../observer//BattleLogger.h"
#include "../core/ThreadSafeConsole.h"

class AttackVisitor : public Visitor {
private:
    std::shared_ptr<NPC> target;
    std::shared_ptr<BattleLogger> logger;

    void logAttack(const std::string& attackerType, const std::string& attackerID) {
        if (logger) {
            std::string event = attackerID + " атакует " + target->getID();
            logger->logGameEvent(event);
        }
    }

public:
    AttackVisitor(std::shared_ptr<NPC> target, std::shared_ptr<BattleLogger> logger = nullptr) 
        : target(target), logger(logger) {}

    void visit(Orc& attacker) override {
        logAttack("Орк", attacker.getID());
    }

    void visit(Squirrel& attacker) override {
        logAttack("Белка", attacker.getID());
    }

    void visit(Bear& attacker) override {
        logAttack("Медведь", attacker.getID());
    }
};