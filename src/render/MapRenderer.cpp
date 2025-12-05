#include "../../include/render/MapRenderer.h"
#include <iomanip>
#include <sstream>

MapRender::RenderData MapRender::collectRenderData(const std::vector<std::shared_ptr<NPC>>& npcs, int elapsedSeconds, int remainingSeconds) {
    RenderData data;
    data.npcs = npcs;
    data.elapsedSeconds = elapsedSeconds;
    data.remainingSeconds = remainingSeconds;
    
    data.orcsCount = 0;
    data.squirrelsCount = 0;
    data.bearsCount = 0;
    data.aliveCount = 0;
    
    for (const auto& npc : npcs) {
        if (npc->isAlive()) {
            data.aliveCount++;
            std::string type = npc->getType();
            if (type == "Орк") data.orcsCount++;
            else if (type == "Белка") data.squirrelsCount++;
            else if (type == "Медведь") data.bearsCount++;
        }
    }
    
    return data;
}

void MapRender::printMap(const std::vector<std::vector<char>>& map, const RenderData& data) {
    std::ostringstream oss;
    
    // Статистика
    oss << "\n=== СТАТИСТИКА ===\n";
    oss << "Время игры: " << data.elapsedSeconds << "с\n";
    oss << "Время до конца: " << data.remainingSeconds << "с\n";
    oss << "Живых NPC: " << data.aliveCount << "\n";
    oss << "- Орки: " << data.orcsCount << "\n";
    oss << "- Белки: " << data.squirrelsCount << "\n";
    oss << "- Медведи: " << data.bearsCount << "\n";
    oss << "==================\n\n";
    
    // Карта
    oss << "+";
    for (int x = 0; x < width; ++x) oss << "-";
    oss << "+\n";

    for (int y = 0; y < height; ++y) {
        oss << "|";
        for (int x = 0; x < width; ++x) {
            oss << map[y][x];
        }
        oss << "|\n";
    }

    oss << "+";
    for (int x = 0; x < width; ++x) oss << "-";
    oss << "+\n";

    oss << "\nЛегенда:\n";
    oss << " O - Орк\n";
    oss << " S - Белка\n";
    oss << " B - Медведь\n";
    oss << " X - Несколько NPC\n";
    oss << " . - Пустая клетка\n";

    ThreadSafeConsole::printStream(oss);
}

void MapRender::renderGameState(const std::vector<std::shared_ptr<NPC>>& npcs, int elapsedSeconds, int remainingSeconds) {

    RenderData data = collectRenderData(npcs, elapsedSeconds, remainingSeconds);
    std::vector<std::vector<char>> map(height, std::vector<char>(width, EMPRY_CELL));
    
    for (const auto& npc : data.npcs) {
        if (npc->isAlive()) {
            Point pos = npc->getPos();
            if (pos.getX() >= 0 && pos.getX() < width && 
                pos.getY() >= 0 && pos.getY() < height) {
                if (map[pos.getY()][pos.getX()] != EMPRY_CELL) {
                    map[pos.getY()][pos.getX()] = MULTIPLE_NPC;
                } else {
                    map[pos.getY()][pos.getX()] = npc->getSymbol();
                }
            }
        }
    }
    
    printMap(map, data);
}

void MapRender::renderFinalState(const std::vector<std::shared_ptr<NPC>>& npcs, int totalSeconds, int initialCount) {

    RenderData data = collectRenderData(npcs, totalSeconds, 0);
    std::ostringstream oss;
    
    // Красивый заголовок
    oss << "\n";
    oss << "╔══════════════════════════════════════════╗\n";
    oss << "║         ФИНАЛЬНЫЕ РЕЗУЛЬТАТЫ            ║\n";
    oss << "╚══════════════════════════════════════════╝\n\n";
    
    // Общая статистика
    oss << "📊 ОБЩАЯ СТАТИСТИКА:\n";
    oss << "   Время игры: " << data.elapsedSeconds << " секунд\n";
    oss << "   Начало: " << initialCount << " NPC\n";
    oss << "   Конец: " << data.aliveCount << " NPC\n";
    oss << "   Выжило: " << std::fixed << std::setprecision(1) 
        << (data.aliveCount * 100.0 / initialCount) << "%\n\n";
    
    // Распределение
    oss << "🎯 РАСПРЕДЕЛЕНИЕ ПО ТИПАМ:\n";
    oss << "   Орки: " << data.orcsCount << "\n";
    oss << "   Белки: " << data.squirrelsCount << "\n";
    oss << "   Медведи: " << data.bearsCount << "\n\n";
    
    // Карта
    oss << "🗺️  ФИНАЛЬНАЯ КАРТА:\n";
    
    // Создаем карту
    std::vector<std::vector<char>> map(height, std::vector<char>(width, EMPRY_CELL));
    for (const auto& npc : data.npcs) {
        if (npc->isAlive()) {
            Point pos = npc->getPos();
            if (pos.getX() >= 0 && pos.getX() < width && 
                pos.getY() >= 0 && pos.getY() < height) {
                if (map[pos.getY()][pos.getX()] != EMPRY_CELL) {
                    map[pos.getY()][pos.getX()] = MULTIPLE_NPC;
                } else {
                    map[pos.getY()][pos.getX()] = npc->getSymbol();
                }
            }
        }
    }
    
    // Выводим карту
    oss << "+";
    for (int x = 0; x < width; ++x) oss << "-";
    oss << "+\n";

    for (int y = 0; y < height; ++y) {
        oss << "|";
        for (int x = 0; x < width; ++x) {
            oss << map[y][x];
        }
        oss << "|\n";
    }

    oss << "+";
    for (int x = 0; x < width; ++x) oss << "-";
    oss << "+\n";

    // oss << "\nЛегенда:\n";
    // oss << " O - Орк\n";
    // oss << " S - Белка\n";
    // oss << " B - Медведь\n";
    // oss << " X - Несколько NPC\n";
    // oss << " . - Пустая клетка\n";

    ThreadSafeConsole::printStream(oss);
}