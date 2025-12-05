#pragma once

#include "../npc/NPC.h"
#include "../core/ThreadSafeConsole.h"

#include <vector>

class MapRender {
private:
    const int width = GameConfig::MAP_WIDTH;
    const int height = GameConfig::MAP_HEIGHT;
    const char EMPRY_CELL = '.';
    const char MULTIPLE_NPC = 'X';

    struct RenderData {
        std::vector<std::shared_ptr<NPC>> npcs;
        int elapsedSeconds;
        int remainingSeconds;
        int orcsCount;
        int squirrelsCount;
        int bearsCount;
        int aliveCount;
    };

public:
    RenderData collectRenderData(const std::vector<std::shared_ptr<NPC>>& npcs, int elapsedSeconds, int remainingSeconds);
    
    void printMap(const std::vector<std::vector<char>>& map, const RenderData& data);

    void render(const std::vector<std::shared_ptr<NPC>>& npcs);
    void renderStats(const std::vector<std::shared_ptr<NPC>>& npcs, int elapsedSeconds, int remainingSeconds);
    
    // всё в одном
    void renderGameState(const std::vector<std::shared_ptr<NPC>>& npcs, int elapsedSeconds, int remainingSeconds);            
    void renderFinalState(const std::vector<std::shared_ptr<NPC>>& npcs, int totalSeconds, int initialCount);
};
