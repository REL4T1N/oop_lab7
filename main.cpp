#include "./include/core/Game.h"

int main() {
    try {
        TS_PRINTLN("=== Сражение NPC ===");
        TS_PRINTLN("Настройки игры:");
        TS_PRINTLN("  Размер карты: " + std::to_string(GameConfig::MAP_WIDTH) + 
                  "x" + std::to_string(GameConfig::MAP_HEIGHT));
        TS_PRINTLN("  Длительность игры: " + 
                  std::to_string(GameConfig::GAME_DURATION.count()) + " секунд");
        TS_PRINTLN("  Создано NPC: " + std::to_string(GameConfig::INITIAL_NPC_COUNT));
        TS_PRINTLN("============================\n");
        
        TS_PRINTLN("Нажмите Enter для запуска...");
        std::cin.get();
        
        // Создаем и запускаем игру
        Game game;
        game.run();
        
        TS_PRINTLN("\nНажмите Enter для выхода...");
        std::cin.get();
        
    } catch (const std::exception& e) {
        TS_PRINTLN("Fatal error: " + std::string(e.what()));
        return 1;
    }
    
    return 0;
}