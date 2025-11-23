#pragma once

#include "./NPCCreator.h"
#include "../npc/elf.h"


class ElfCreator : public INPCCreator {
public:
    std::unique_ptr<INPC> createNPC(const std::string& name, int x, int y, int range) override;
};