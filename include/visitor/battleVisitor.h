#pragma once

#include "./visitor.h"
#include "../npc/NPC.h"
#include "../observer/battleLogger.h"
#include <iostream>


class BattleVisitor : public Visitor {
private:
    INPC* attacker;
    INPC* defender;
    bool fightOccurred;     // Был ли бой вообще
    INPC* winner;
    INPC* loser;
    std::shared_ptr<BattleLogger> logger;

    void resolveFight(INPC* winner, INPC* loser);

public:
    BattleVisitor(INPC* att, INPC* def, std::shared_ptr<BattleLogger> battleLogger = nullptr);

    void visit(Elf& elf) override;
    void visit(Dragon& dragon) override;
    void visit(Druid& druid) override;

    bool didFightOccur() const;
    INPC* getWinner() const;
    INPC* getLoser() const;
};
