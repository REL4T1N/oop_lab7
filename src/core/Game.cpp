#include "../../include/core/Game.h"

Game::Game() {
    initialize();
}

Game::~Game() {
    stop();
}

void Game::initialize() {
    TS_PRINTLN("=== Создание игры ===");

    world = std::make_shared<GameWorld>();
    dice = std::make_shared<Dice>();
    battle_logger = std::make_shared<BattleLogger>();

    coroutine_manager = std::make_shared<CoroutineManager>(world, dice, battle_logger);
    factory = std::make_shared<NPCFactory>();
    renderer = std::make_shared<MapRender>();
    timer = std::make_shared<GameTimer>();

    auto console_logger = std::make_shared<ConsoleLogger>();
    auto file_logger = std::make_shared<FileLogger>("../data/battle_log.txt");

    battle_logger->attach(console_logger);
    battle_logger->attach(file_logger);

    createInitialNPCs();

    TS_PRINTLN("=== Игра создана ===");
}

void Game::createInitialNPCs() {
    TS_PRINTLN("Создание " + std::to_string(GameConfig::INITIAL_NPC_COUNT) + " NPC");

    for (int i = 0; i < GameConfig::INITIAL_NPC_COUNT; ++i) {
        try {
            auto npc = factory->createRandomNPC();
            world->addNPC(npc);

        auto behavior = createNPCBehavior(npc, world, dice, battle_logger);
        coroutine_manager->addNPC(npc);
        } catch (const std::exception& e) {
            TS_PRINTLN("Ошибка при создании NPC: " + std::string(e.what()));
        }
    }

    TS_PRINTLN("Создано " + std::to_string(GameConfig::INITIAL_NPC_COUNT) + " NPC");
}

void Game::renderLoop() {
    TS_PRINTLN("=== RENDER LOOP STARTED ===");

    try {
        while (running && timer->shouldContinue()) {
            auto start_time = std::chrono::steady_clock::now();

            auto alive_npcs = coroutine_manager->getAliveNPCs();
            int elapsed = timer->getElapsedTime().count();
            int remaining = timer->getRemainingTime().count();

            renderer->renderGameState(alive_npcs, elapsed, remaining);
            world->cleanupDeadNPCs();

            auto end_time = std::chrono::steady_clock::now();
            auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

            if (elapsed_ms < GameConfig::RENDER_INTERVAL) {
                std::this_thread::sleep_for(GameConfig::RENDER_INTERVAL - elapsed_ms);
            }
        }
    }
    catch (const std::exception& e) {
        TS_PRINTLN("Ошибка в renderLoop: " + std::string(e.what()));
    }
    
    TS_PRINTLN("=== RENDER LOOP ENDED ===");
}


void Game::finishGame() {
    std::cout << "[GAME DEBUG] finishGame called" << std::endl;
    
    running = false;
    game_over = true;
    
    // Останавливаем таймер
    timer->stop();

    // Останавливаем корутины
    coroutine_manager->stop();
    
    // Ждем завершения потока рендера
    if (render_thread.joinable()) {
        std::cout << "[GAME DEBUG] Joining render thread" << std::endl;
        render_thread.join();
    }

    // Очистка экрана
    std::cout << "\033[2J\033[1;1H";
    
    // Финальный рендер
    auto final_npcs = coroutine_manager->getAliveNPCs();
    int elapsed = timer->getElapsedTime().count();
    
    renderer->renderFinalState(final_npcs, elapsed, GameConfig::INITIAL_NPC_COUNT);
    
    // Дополнительная информация
    std::cout << "\n🏆 ВЫЖИВШИЕ NPC:\n";
    for (const auto& npc : final_npcs) {
        Point pos = npc->getPos();
        std::cout << "  • " << npc->getID() << " (" << npc->getType() << ")\n";
        std::cout << "    Позиция: (" << pos.getX() << ", " << pos.getY() << ")\n";
    }

    
    printFinalStats();
    saveGameStats();
    
    std::cout << "[GAME DEBUG] Game finished completely" << std::endl;
}

void Game::stop() {
    if (running) {
        running = false;
        coroutine_manager->stop();

        if (render_thread.joinable()) {
            render_thread.join();
        }
    }
}

void Game::printFinalStats() {
    auto alive_npcs = coroutine_manager->getAliveNPCs();

    TS_PRINTLN("\n === Итоговая статистика ===");
    TS_PRINTLN("Игра длилась: " + std::to_string(GameConfig::GAME_DURATION.count()) + "секунд");
    TS_PRINTLN("Выжило: " + std::to_string(alive_npcs.size()));

    if (!alive_npcs.empty()) {
        TS_PRINTLN("\nЖивые NPC:");
        for (const auto& npc : alive_npcs) {
            Point pos = npc->getPos();
            TS_PRINTLN("  " + npc->getID() + " (" + npc->getType() + ") расположен на (" + std::to_string(pos.getX()) + ", " + std::to_string(pos.getY()) + ")");
        }
    }
    TS_PRINTLN("=================================\n");
}

void Game::saveGameStats() {
    try {
        auto alive_npcs =  coroutine_manager->getAliveNPCs();
        factory->saveToFile("../data/game_save.txt", alive_npcs);
        TS_PRINTLN("Итоги игры сохранены в '../data/game_save.txt'");
    } catch (const std::exception& e) {
        TS_PRINTLN("Ошибка сохранения игры: " + std::string(e.what()));
    }
}

void Game::run() {
    running = true;
    
    // Запускаем таймер ПЕРВЫМ
    timer->start();
    
    // Очищаем консоль
    std::cout << "\033[2J\033[1;1H";
    
    TS_PRINTLN("=== ИГРА НАЧАЛАСЬ ===");
    TS_PRINTLN("Время игры: " + std::to_string(GameConfig::GAME_DURATION.count()) + " секунд");
    
    // Запускаем корутины
    coroutine_manager->start();
    
    // Запускаем рендер в отдельном потоке
    render_thread = std::thread([this]() { renderLoop(); });
    
    // ПРОСТОЙ цикл ожидания окончания времени
    try {
        while (timer->shouldContinue() && running) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            
            // Опционально: проверка на отсутствие живых NPC
            auto alive_npcs = coroutine_manager->getAliveNPCs();
            if (alive_npcs.empty()) {
                TS_PRINTLN("\n=== Все NPC мертвы ===");
                break;
            }
        }
    }
    catch (const std::exception& e) {
        TS_PRINTLN("Ошибка в основном цикле: " + std::string(e.what()));
    }
    
    // Завершаем игру
    finishGame();
}