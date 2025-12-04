#pragma once

#include "./Subject.h"
#include "../npc/NPC.h"
#include "../core/ThreadSafeConsole.h"

class BattleLogger : public ISubject {
public:
    void logCombat(std::shared_ptr<NPC> attacker, std::shared_ptr<NPC> defender, bool attackerWon);
    void logGameEvent(const std::string& event);
};