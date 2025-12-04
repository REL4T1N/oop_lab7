#pragma once

#include "./NPCCreator.h"
#include "../npc/Squirrel.h"

class SquirrelCreator : public NPCCreator {
public:
    std::shared_ptr<NPC> createNPC(const std::string& id) override;
    std::shared_ptr<NPC> createNPC(const std::string& id, const Point& p) override;
    std::shared_ptr<NPC> createNPC(const std::string& id, const Point& p, int moveRange, int attackRange) override;
};