#include "../../include/factory/NPCCreator.h"

Point NPCCreator::generateRandomPosition() const {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> xDist(0, GameConfig::MAP_WIDTH - 1);
    std::uniform_int_distribution<> yDist(0, GameConfig::MAP_HEIGHT - 1);

    return Point(xDist(gen), yDist(gen));
}