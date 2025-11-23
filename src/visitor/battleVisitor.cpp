#include "../../include/visitor/battleVisitor.h"
#include "../../include/npc/elf.h"
#include "../../include/npc/dragon.h"
#include "../../include/npc/druid.h"

BattleVisitor::BattleVisitor(INPC* att, INPC* def, std::shared_ptr<BattleLogger> battleLogger) : attacker(att), defender(def), fightOccurred(false), winner(nullptr), loser(nullptr), logger(battleLogger) {}

bool BattleVisitor::didFightOccur() const {
    return fightOccurred;
}

INPC* BattleVisitor::getWinner() const {
    return winner;
}

INPC* BattleVisitor::getLoser() const {
    return loser;
}

void BattleVisitor::resolveFight(INPC* winnerNPC, INPC* loserNPC) {
    winner = winnerNPC;
    loser = loserNPC;
    fightOccurred = true;

    if (logger) {
        logger->logKill(winner, loser);
    } else {
        std::cout << winner->getName() << " (" << winner->getType() << ") победил " << loser->getName() << " (" << loser->getType() << ")!" << std::endl;
    }
}

void BattleVisitor::visit(Elf& elf) {
    if (!attacker->isAlive() || !defender->isAlive()) {
        return;
    }

    // Эльф атакует только Друидов
    if (defender->getType() == "Друид") {
        resolveFight(attacker, defender);
    }
}

void BattleVisitor::visit(Dragon& dragon) {
    if (!attacker->isAlive() || !defender->isAlive()) {
        return;
    }

    // Дракон атакует только Эльфов
    if (defender->getType() == "Эльф") {
        resolveFight(attacker, defender);
    }
}

void BattleVisitor::visit(Druid& druid) {
    if (!attacker->isAlive() || !defender->isAlive()) {
        return;
    }

    // Друид атакует только Драконов
    if (defender->getType() == "Дракон") {
        resolveFight(attacker, defender);
    }
}