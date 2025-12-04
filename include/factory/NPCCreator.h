#pragma once

#include "../npc/NPC.h"
#include <random>

class NPCCreator {
public:
    virtual ~NPCCreator() = default;
    
    virtual std::shared_ptr<NPC> createNPC(const std::string& id) = 0;
    virtual std::shared_ptr<NPC> createNPC(const std::string& id, const Point& p) = 0;
    virtual std::shared_ptr<NPC> createNPC(const std::string& id, const Point& p, int moveRange, int attackRange) = 0;

protected:
    Point generateRandomPosition() const;
};