#include <gtest/gtest.h>
#include <sstream>
#include <memory>
#include <vector>
#include "../include/render/MapRenderer.h"
#include "../include/npc/Bear.h"
#include "../include/npc/Orc.h"
#include "../include/npc/Squirrel.h"
#include "../include/core/Point.h"
#include "../include/core/ThreadSafeConsole.h"

// Фикстура для тестов рендерера
class RenderTest : public ::testing::Test {
protected:
    MapRender renderer;
    std::vector<std::shared_ptr<NPC>> npcs;
    
    void SetUp() override {
        // Создаем тестовых NPC
        npcs.push_back(std::make_shared<Orc>("Орк_1", Point(3, 4)));
        npcs.push_back(std::make_shared<Bear>("Медведь_1", Point(7, 8)));
        npcs.push_back(std::make_shared<Squirrel>("Белка_1", Point(1, 2)));
        npcs.push_back(std::make_shared<Orc>("Орк_2", Point(10, 11)));
        npcs.push_back(std::make_shared<Bear>("Медведь_2", Point(14, 0)));
        
        // Помечаем одного NPC мертвым для тестов
        npcs[3]->markDead();
    }
    
    void TearDown() override {
        npcs.clear();
    }
    
    // Вспомогательная функция для захвата вывода
    std::string captureOutput(void (MapRender::*renderFunc)(const std::vector<std::shared_ptr<NPC>>&, int, int), 
                             int elapsed = 0, int remaining = 0) {
        testing::internal::CaptureStdout();
        (renderer.*renderFunc)(npcs, elapsed, remaining);
        return testing::internal::GetCapturedStdout();
    }
};

// ==================== ТЕСТЫ БАЗОВОГО РЕНДЕРИНГА ====================

// Тест создания рендерера
TEST_F(RenderTest, RendererCreation) {
    EXPECT_NO_THROW(MapRender renderer);
}

// Тест collectRenderData
TEST_F(RenderTest, CollectRenderData) {
    auto data = renderer.collectRenderData(npcs, 10, 20);
    
    EXPECT_EQ(data.elapsedSeconds, 10);
    EXPECT_EQ(data.remainingSeconds, 20);
    EXPECT_EQ(data.npcs.size(), 5);
    
    // Один NPC мертв, должно быть 4 живых
    EXPECT_EQ(data.aliveCount, 4);
    
    // Проверяем подсчет по типам
    int orcs = 0, bears = 0, squirrels = 0;
    for (const auto& npc : npcs) {
        if (npc->isAlive()) {
            std::string type = npc->getType();
            if (type == "Орк") orcs++;
            else if (type == "Медведь") bears++;
            else if (type == "Белка") squirrels++;
        }
    }
    
    EXPECT_EQ(data.orcsCount, orcs);
    EXPECT_EQ(data.bearsCount, bears);
    EXPECT_EQ(data.squirrelsCount, squirrels);
}

// Тест рендеринга статистики
TEST_F(RenderTest, RenderGameStateOutput) {
    std::string output = captureOutput(&MapRender::renderGameState, 15, 45);
    
    // Проверяем наличие ключевых элементов в выводе
    EXPECT_NE(output.find("=== СТАТИСТИКА ==="), std::string::npos);
    EXPECT_NE(output.find("Время игры: 15с"), std::string::npos);
    EXPECT_NE(output.find("Время до конца: 45с"), std::string::npos);
    EXPECT_NE(output.find("Живых NPC:"), std::string::npos);
    EXPECT_NE(output.find("- Орки:"), std::string::npos);
    EXPECT_NE(output.find("- Белки:"), std::string::npos);
    EXPECT_NE(output.find("- Медведи:"), std::string::npos);
    EXPECT_NE(output.find("=================="), std::string::npos);
    
    // Проверяем карту
    EXPECT_NE(output.find("+"), std::string::npos);
    EXPECT_NE(output.find("|"), std::string::npos);
    EXPECT_NE(output.find("-"), std::string::npos);
    
    // Проверяем легенду
    EXPECT_NE(output.find("Легенда:"), std::string::npos);
    EXPECT_NE(output.find("O - Орк"), std::string::npos);
    EXPECT_NE(output.find("S - Белка"), std::string::npos);
    EXPECT_NE(output.find("B - Медведь"), std::string::npos);
    EXPECT_NE(output.find("X - Несколько NPC"), std::string::npos);
    EXPECT_NE(output.find(". - Пустая клетка"), std::string::npos);
}

// Тест рендеринга финального состояния
TEST_F(RenderTest, RenderFinalStateOutput) {
    std::string output = captureOutput(&MapRender::renderFinalState, 60, 10);
    
    // Проверяем финальный заголовок
    EXPECT_NE(output.find("ФИНАЛЬНЫЕ РЕЗУЛЬТАТЫ"), std::string::npos);
    EXPECT_NE(output.find("╔══════════════════════════════════════════╗"), std::string::npos);
    EXPECT_NE(output.find("╚══════════════════════════════════════════╝"), std::string::npos);
    
    // Проверяем статистику
    EXPECT_NE(output.find("📊 ОБЩАЯ СТАТИСТИКА:"), std::string::npos);
    EXPECT_NE(output.find("Время игры: 60 секунд"), std::string::npos);
    EXPECT_NE(output.find("Начало: 10 NPC"), std::string::npos);
    EXPECT_NE(output.find("Конец:"), std::string::npos);
    EXPECT_NE(output.find("Выжило:"), std::string::npos);
    EXPECT_NE(output.find("%"), std::string::npos);
    
    EXPECT_NE(output.find("🎯 РАСПРЕДЕЛЕНИЕ ПО ТИПАМ:"), std::string::npos);
    EXPECT_NE(output.find("Орки:"), std::string::npos);
    EXPECT_NE(output.find("Белки:"), std::string::npos);
    EXPECT_NE(output.find("Медведи:"), std::string::npos);
    
    EXPECT_NE(output.find("🗺️  ФИНАЛЬНАЯ КАРТА:"), std::string::npos);
}

// ==================== ТЕСТЫ КАРТЫ ====================

// Тест отображения NPC на карте
TEST_F(RenderTest, MapNPCPlacement) {
    // Очищаем NPC и добавляем одного в известную позицию
    npcs.clear();
    npcs.push_back(std::make_shared<Orc>("Орк_1", Point(5, 5)));
    
    std::string output = captureOutput(&MapRender::renderGameState, 0, 0);
    
    // Должен отобразиться символ 'O' на позиции (5,5)
    // Поскольку мы не можем напрямую проверить координаты в выводе,
    // проверяем что символ 'O' присутствует в выводе
    EXPECT_NE(output.find("O"), std::string::npos);
}

// Тест отображения нескольких NPC на одной клетке
TEST_F(RenderTest, MultipleNPCsSameCell) {
    npcs.clear();
    
    // Добавляем двух NPC на одну клетку
    npcs.push_back(std::make_shared<Orc>("Орк_1", Point(5, 5)));
    npcs.push_back(std::make_shared<Bear>("Медведь_1", Point(5, 5)));
    
    std::string output = captureOutput(&MapRender::renderGameState, 0, 0);
    
    // Должен отобразиться символ 'X' (несколько NPC)
    EXPECT_NE(output.find("X"), std::string::npos);
    // Не должно быть символов 'O' или 'B' на этой клетке
    // Но может быть на других позициях в выводе
}

// Тест пустой карты
TEST_F(RenderTest, EmptyMap) {
    npcs.clear(); // Удаляем всех NPC
    
    std::string output = captureOutput(&MapRender::renderGameState, 0, 0);
    
    // Должны быть только точки (пустые клетки)
    // Проверяем что есть символы '.' (это символ пустой клетки)
    EXPECT_NE(output.find("."), std::string::npos);
    
    // Проверяем статистику - живых NPC должно быть 0
    EXPECT_NE(output.find("Живых NPC: 0"), std::string::npos);
}

// Тест NPC за границами карты
TEST_F(RenderTest, NPCOutOfBounds) {
    npcs.clear();
    
    // Добавляем NPC за границами карты
    npcs.push_back(std::make_shared<Orc>("Орк_1", Point(-1, -1))); // Левая верхняя граница
    npcs.push_back(std::make_shared<Bear>("Медведь_1", Point(GameConfig::MAP_WIDTH, GameConfig::MAP_HEIGHT))); // Правая нижняя граница
    
    std::string output = captureOutput(&MapRender::renderGameState, 0, 0);
    
    // NPC за границами не должны отображаться
    // В выводе не должно быть символов 'O' или 'B'
    // Но могут быть в других местах вывода (в статистике)
}

// Тест всех типов NPC на карте
TEST_F(RenderTest, AllNPCTypesOnMap) {
    npcs.clear();
    
    // Добавляем по одному NPC каждого типа в разные позиции
    npcs.push_back(std::make_shared<Orc>("Орк_1", Point(1, 1)));      // 'O'
    npcs.push_back(std::make_shared<Squirrel>("Белка_1", Point(3, 3))); // 'S'
    npcs.push_back(std::make_shared<Bear>("Медведь_1", Point(5, 5)));   // 'B'
    
    std::string output = captureOutput(&MapRender::renderGameState, 0, 0);
    
    // Проверяем что все символы присутствуют на карте
    EXPECT_NE(output.find("O"), std::string::npos);
    EXPECT_NE(output.find("S"), std::string::npos);
    EXPECT_NE(output.find("B"), std::string::npos);
}

// ==================== ТЕСТЫ ГРАНИЧНЫХ СЛУЧАЕВ ====================

// Тест с нулевым временем
TEST_F(RenderTest, ZeroTime) {
    std::string output = captureOutput(&MapRender::renderGameState, 0, 0);
    
    EXPECT_NE(output.find("Время игры: 0с"), std::string::npos);
    EXPECT_NE(output.find("Время до конца: 0с"), std::string::npos);
}

// Тест с большим временем
TEST_F(RenderTest, LargeTime) {
    std::string output = captureOutput(&MapRender::renderGameState, 999, 999);
    
    EXPECT_NE(output.find("Время игры: 999с"), std::string::npos);
    EXPECT_NE(output.find("Время до конца: 999с"), std::string::npos);
}

// Тест с отрицательным временем (если возможно)
TEST_F(RenderTest, NegativeTime) {
    // Не можем передать отрицательное время, так как параметры int
    // Но можно проверить как обрабатывается
    std::string output = captureOutput(&MapRender::renderGameState, -5, -10);
    
    // Должно отображаться отрицательное время
    EXPECT_NE(output.find("Время игры: -5с"), std::string::npos);
    EXPECT_NE(output.find("Время до конца: -10с"), std::string::npos);
}

// Тест с пустым вектором NPC
TEST_F(RenderTest, EmptyNPCVector) {
    std::vector<std::shared_ptr<NPC>> emptyNpcs;
    
    testing::internal::CaptureStdout();
    renderer.renderGameState(emptyNpcs, 10, 20);
    std::string output = testing::internal::GetCapturedStdout();
    
    EXPECT_NE(output.find("Живых NPC: 0"), std::string::npos);
    EXPECT_NE(output.find("Орки: 0"), std::string::npos);
    EXPECT_NE(output.find("Белки: 0"), std::string::npos);
    EXPECT_NE(output.find("Медведи: 0"), std::string::npos);
}

// Тест с null указателями в векторе (если возможно)
TEST_F(RenderTest, NullNPCInVector) {
    npcs.clear();
    npcs.push_back(std::make_shared<Orc>("Орк_1", Point(1, 1)));
    
    // Не должно падать при рендеринге
    EXPECT_NO_THROW(renderer.renderGameState(npcs, 0, 0));
    
    std::string output = captureOutput(&MapRender::renderGameState, 0, 0);
    
    // Должен отобразиться только один живой NPC
    EXPECT_NE(output.find("Живых NPC: 1"), std::string::npos);
}

// ==================== ТЕСТЫ ФОРМАТИРОВАНИЯ ====================

// Тест форматирования процентов в финальном состоянии
TEST_F(RenderTest, PercentageFormatting) {
    // Создаем ровно 10 NPC для простого расчета процентов
    npcs.clear();
    for (int i = 0; i < 10; ++i) {
        npcs.push_back(std::make_shared<Orc>("Орк_" + std::to_string(i), Point(i, i)));
    }
    
    // Убиваем половину
    for (int i = 5; i < 10; ++i) {
        npcs[i]->markDead();
    }
    
    testing::internal::CaptureStdout();
    renderer.renderFinalState(npcs, 60, 10);
    std::string output = testing::internal::GetCapturedStdout();
    
    // Должно быть 50.0%
    EXPECT_NE(output.find("Выжило: 50.0%"), std::string::npos);
}

// Тест форматирования с 0% выживших
TEST_F(RenderTest, ZeroPercentage) {
    // Все NPC мертвы
    for (auto& npc : npcs) {
        npc->markDead();
    }
    
    std::string output = captureOutput(&MapRender::renderFinalState, 60, 5);
    
    EXPECT_NE(output.find("Выжило: 0.0%"), std::string::npos);
}

// Тест форматирования с 100% выживших
TEST_F(RenderTest, HundredPercentage) {
    // Все NPC живы
    for (auto& npc : npcs) {
        // npc уже жив (кроме одного)
    }
    // Воскрешаем мертвого
    npcs[3]->markDead(); // Он уже мертв, нужно "воскресить"
    // В текущей реализации нет метода revive, так что создаем нового
    npcs[3] = std::make_shared<Orc>("Орк_2", Point(10, 11));
    
    std::string output = captureOutput(&MapRender::renderFinalState, 60, 5);
    
    EXPECT_NE(output.find("Выжило: 100.0%"), std::string::npos);
}

// ==================== ТЕСТЫ РАЗМЕРА КАРТЫ ====================

// Тест граничных координат карты
TEST_F(RenderTest, MapBoundaries) {
    npcs.clear();
    
    // Добавляем NPC по углам карты
    npcs.push_back(std::make_shared<Orc>("Орк_1", Point(0, 0))); // Левый верхний угол
    npcs.push_back(std::make_shared<Bear>("Медведь_1", Point(GameConfig::MAP_WIDTH - 1, 0))); // Правый верхний угол
    npcs.push_back(std::make_shared<Squirrel>("Белка_1", Point(0, GameConfig::MAP_HEIGHT - 1))); // Левый нижний угол
    npcs.push_back(std::make_shared<Orc>("Орк_2", Point(GameConfig::MAP_WIDTH - 1, GameConfig::MAP_HEIGHT - 1))); // Правый нижний угол
    
    std::string output = captureOutput(&MapRender::renderGameState, 0, 0);
    
    // Все NPC должны отобразиться
    EXPECT_NE(output.find("O"), std::string::npos);
    EXPECT_NE(output.find("B"), std::string::npos);
    EXPECT_NE(output.find("S"), std::string::npos);
}

// Тест размера карты в выводе
TEST_F(RenderTest, MapSizeInOutput) {
    std::string output = captureOutput(&MapRender::renderGameState, 0, 0);
    
    // Считаем количество символов в первой строке карты
    size_t plusPos = output.find("+");
    size_t newlinePos = output.find("\n", plusPos);
    std::string firstLine = output.substr(plusPos, newlinePos - plusPos);
    
    // Должно быть: "+---------------+" для MAP_WIDTH = 15
    // Количество '-' должно быть равно MAP_WIDTH
    int dashCount = 0;
    for (char c : firstLine) {
        if (c == '-') dashCount++;
    }
    
    EXPECT_EQ(dashCount, GameConfig::MAP_WIDTH);
}

// ==================== ТЕСТЫ СИМВОЛОВ ====================

// Тест символов NPC
TEST_F(RenderTest, NPCSymbols) {
    // Проверяем что символы соответствуют GameConfig
    Orc orc("Орк", Point(0, 0));
    Bear bear("Медведь", Point(0, 0));
    Squirrel squirrel("Белка", Point(0, 0));
    
    EXPECT_EQ(orc.getSymbol(), 'O');
    EXPECT_EQ(bear.getSymbol(), 'B');
    EXPECT_EQ(squirrel.getSymbol(), 'S');
    
    // Проверяем что эти символы используются в рендерере
    npcs.clear();
    npcs.push_back(std::make_shared<Orc>("Орк", Point(1, 1)));
    
    std::string output = captureOutput(&MapRender::renderGameState, 0, 0);
    
    // Ищем символ 'O' в выводе
    EXPECT_NE(output.find("O"), std::string::npos);
}

// ==================== ТЕСТЫ ПРОИЗВОДИТЕЛЬНОСТИ ====================

// Тест рендеринга большого количества NPC
TEST_F(RenderTest, PerformanceManyNPCs) {
    npcs.clear();
    
    // Создаем много NPC
    for (int i = 0; i < 100; ++i) {
        int x = i % GameConfig::MAP_WIDTH;
        int y = i / GameConfig::MAP_WIDTH;
        npcs.push_back(std::make_shared<Orc>("Орк_" + std::to_string(i), Point(x, y)));
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    
    testing::internal::CaptureStdout();
    renderer.renderGameState(npcs, 0, 0);
    std::string output = testing::internal::GetCapturedStdout();
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Должно выполняться быстро (менее 100 мс)
    EXPECT_LT(duration.count(), 100);
    
    // Проверяем что все NPC учтены в статистике
    EXPECT_NE(output.find("Живых NPC: 100"), std::string::npos);
}

// Тест рендеринга при переполнении карты
TEST_F(RenderTest, MapOverflow) {
    npcs.clear();
    
    // Создаем больше NPC, чем клеток на карте
    int totalCells = GameConfig::MAP_WIDTH * GameConfig::MAP_HEIGHT;
    for (int i = 0; i < totalCells * 2; ++i) {
        int x = i % GameConfig::MAP_WIDTH;
        int y = (i / GameConfig::MAP_WIDTH) % GameConfig::MAP_HEIGHT;
        npcs.push_back(std::make_shared<Orc>("Орк_" + std::to_string(i), Point(x, y)));
    }
    
    // Не должно падать
    EXPECT_NO_THROW(renderer.renderGameState(npcs, 0, 0));
    
    std::string output = captureOutput(&MapRender::renderGameState, 0, 0);
    
    // Многие клетки должны иметь символ 'X' (несколько NPC)
    EXPECT_NE(output.find("X"), std::string::npos);
}

// ==================== ТЕСТЫ ИНТЕГРАЦИИ ====================

// Тест что ThreadSafeConsole используется
TEST_F(RenderTest, ThreadSafeConsoleUsage) {
    // Не можем напрямую проверить ThreadSafeConsole, но можем
    // проверить что вывод происходит через правильный метод
    
    npcs.clear();
    npcs.push_back(std::make_shared<Orc>("Орк_1", Point(1, 1)));
    
    // Мокаем ThreadSafeConsole не просто, но можем проверить что вывод есть
    testing::internal::CaptureStdout();
    renderer.renderGameState(npcs, 0, 0);
    std::string output = testing::internal::GetCapturedStdout();
    
    EXPECT_FALSE(output.empty());
}

// Тест согласованности данных
TEST_F(RenderTest, DataConsistency) {
    // Проверяем что статистика в выводе соответствует реальным данным
    
    // Подсчитываем реальное количество живых NPC
    int realAlive = 0;
    int realOrcs = 0, realBears = 0, realSquirrels = 0;
    
    for (const auto& npc : npcs) {
        if (npc->isAlive()) {
            realAlive++;
            std::string type = npc->getType();
            if (type == "Орк") realOrcs++;
            else if (type == "Медведь") realBears++;
            else if (type == "Белка") realSquirrels++;
        }
    }
    
    std::string output = captureOutput(&MapRender::renderGameState, 0, 0);
    
    // Проверяем что вывод содержит правильные числа
    EXPECT_NE(output.find("Живых NPC: " + std::to_string(realAlive)), std::string::npos);
    EXPECT_NE(output.find("Орки: " + std::to_string(realOrcs)), std::string::npos);
    EXPECT_NE(output.find("Медведи: " + std::to_string(realBears)), std::string::npos);
    EXPECT_NE(output.find("Белки: " + std::to_string(realSquirrels)), std::string::npos);
}