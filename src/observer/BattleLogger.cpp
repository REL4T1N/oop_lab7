#include "../../include/observer/BattleLogger.h"

void BattleLogger::logCombat(std::shared_ptr<NPC> attacker, std::shared_ptr<NPC> defender, bool attackerWon) {
    std::string event;

    if (attackerWon) {
        event = attacker->getID() + " победил " + defender->getID();
    } else {
        event = defender->getID() + " защитился от " + attacker->getID();
    }
    notify(event);
}

void BattleLogger::logGameEvent(const std::string &event) {
    notify("ИГРА: " + event);
}