#pragma once

#include "./NPCCreator.h"
#include "../npc/dragon.h"


class DragonCreator : public INPCCreator {
public:
    std::unique_ptr<INPC> createNPC(const std::string& name, int x, int y, int range) override;
};