#pragma once

#include "./subject.h"
#include "../npc/NPC.h"


class BattleLogger : public ISubject {
public:
    void logKill(INPC* killer, INPC* victim) {
        std::string event = killer->getName() + " (" + killer->getType() + ") убил " + victim->getName() + " (" + victim->getType() + ")";
        notify(event);
    }

    void logBattleStart(int participants) {
        std::string event = "=== НАЧАЛО БОЯ ===";
        notify(event);
    }

    void logBattleEnd(int survivors) {
        std::string event = "=== КОНЕЦ БОЯ. Выжило: " + std::to_string(survivors) + " ===";
        notify(event);
    }
};
