#pragma once

#include "../npc/NPC.h"
#include <memory>

class INPCCreator {
public:
    virtual ~INPCCreator() = default;
    virtual std::unique_ptr<INPC> createNPC(const std::string& name, int x, int y, int range) = 0;
};
