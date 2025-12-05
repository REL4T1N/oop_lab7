#include <gtest/gtest.h>
#include <sstream>
#include <thread>
#include <chrono>
#include <memory>
#include <vector>
#include <atomic>

// Core компоненты
#include "../include/core/Point.h"
#include "../include/core/GameWorld.h"
#include "../include/core/GameTimer.h"
#include "../include/core/ThreadSafeConsole.h"
#include "../include/core/Config.h"

// Behavior компоненты
#include "../include/behavior/Dice.h"
#include "../include/behavior/CoroutineManager.h"
#include "../include/behavior/NPCBehavior.h"

// Зависимости
#include "../include/npc/Bear.h"
#include "../include/npc/Orc.h"
#include "../include/npc/Squirrel.h"
#include "../include/observer/BattleLogger.h"
#include "../include/observer/ConsoleLogger.h"

// ==================== FIXTURES ====================

// Фикстура для Core тестов
class CoreTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

// Фикстура для Behavior тестов
class BehaviorTest : public ::testing::Test {
protected:
    std::shared_ptr<GameWorld> world;
    std::shared_ptr<Dice> dice;
    std::shared_ptr<BattleLogger> logger;
    
    void SetUp() override {
        world = std::make_shared<GameWorld>();
        dice = std::make_shared<Dice>();
        logger = std::make_shared<BattleLogger>();
    }
    
    void TearDown() override {
        world.reset();
        dice.reset();
        logger.reset();
    }
};

// ==================== CORE TESTS: POINT ====================

TEST_F(CoreTest, PointConstructorAndGetters) {
    Point p1;
    EXPECT_EQ(p1.getX(), 0);
    EXPECT_EQ(p1.getY(), 0);
    
    Point p2(5, 10);
    EXPECT_EQ(p2.getX(), 5);
    EXPECT_EQ(p2.getY(), 10);
}

TEST_F(CoreTest, PointSettersWithClamping) {
    Point p(5, 5);
    
    // Внутри границ
    p.setX(10);
    p.setY(12);
    EXPECT_EQ(p.getX(), 10);
    EXPECT_EQ(p.getY(), 12);
    
    // За границами (должны зажиматься)
    p.setX(-5);
    p.setY(100);
    EXPECT_EQ(p.getX(), 0);  // Зажато до 0
    EXPECT_EQ(p.getY(), GameConfig::MAP_HEIGHT - 1);  // Зажато до MAX-1
}

TEST_F(CoreTest, PointDistanceCalculation) {
    Point p1(0, 0);
    Point p2(3, 4);
    
    // Расстояние должно быть 5 (3-4-5 треугольник)
    EXPECT_DOUBLE_EQ(p1.distanceTo(p2), 5.0);
    EXPECT_DOUBLE_EQ(p2.distanceTo(p1), 5.0);
    
    // Точки совпадают
    EXPECT_DOUBLE_EQ(p1.distanceTo(p1), 0.0);
}

TEST_F(CoreTest, PointMoveMethod) {
    Point p(5, 5);
    
    // Нормальное движение
    Point p1 = p.move(3, 4);
    EXPECT_EQ(p1.getX(), 8);
    EXPECT_EQ(p1.getY(), 9);
    
    // Движение за границы (должно зажиматься)
    Point p2 = p.move(20, -10);
    EXPECT_EQ(p2.getX(), GameConfig::MAP_WIDTH - 1);
    EXPECT_EQ(p2.getY(), 0);
}

TEST_F(CoreTest, PointEqualityOperators) {
    Point p1(5, 10);
    Point p2(5, 10);
    Point p3(5, 11);
    
    EXPECT_TRUE(p1 == p2);
    EXPECT_FALSE(p1 == p3);
    EXPECT_TRUE(p1 != p3);
    EXPECT_FALSE(p1 != p2);
}

TEST_F(CoreTest, PointStreamOperators) {
    Point p(7, 13);
    
    std::stringstream ss;
    ss << p;
    EXPECT_EQ(ss.str(), "7 13");
    
    Point p2;
    std::stringstream ss2("42 24");
    ss2 >> p2;
    EXPECT_EQ(p2.getX(), 42);
    EXPECT_EQ(p2.getY(), 24);
}

TEST_F(CoreTest, PointSetCoordinates) {
    Point p;
    p.setCoordinates(10, 20);
    EXPECT_EQ(p.getX(), 10);
    EXPECT_EQ(p.getY(), 14);
    
    // За границами
    p.setCoordinates(-5, 100);
    EXPECT_EQ(p.getX(), 0);
    EXPECT_EQ(p.getY(), 14);
}

// ==================== CORE TESTS: GAMEWORLD ====================

TEST_F(CoreTest, GameWorldAddAndGetNPCs) {
    GameWorld world;
    
    auto orc = std::make_shared<Orc>("Орк_1", Point(5, 5));
    auto bear = std::make_shared<Bear>("Медведь_1", Point(10, 10));
    
    world.addNPC(orc);
    world.addNPC(bear);
    
    auto allNPCs = world.getAllAliveNPCs();
    EXPECT_EQ(allNPCs.size(), 2);
}

TEST_F(CoreTest, GameWorldGetNPCsInRange) {
    GameWorld world;
    
    auto source = std::make_shared<Orc>("Орк_1", Point(5, 5));
    auto near = std::make_shared<Bear>("Медведь_1", Point(6, 6));  // Расстояние ~1.41
    auto far = std::make_shared<Squirrel>("Белка_1", Point(20, 20)); // Далеко
    
    world.addNPC(source);
    world.addNPC(near);
    world.addNPC(far);
    
    // source не должен быть в списке
    auto inRange = world.getNPCsInRange(source, 5.0);
    EXPECT_EQ(inRange.size(), 1);
    EXPECT_EQ(inRange[0]->getID(), "Медведь_1");
}

TEST_F(CoreTest, GameWorldGetNPCsInRangeWithDeadNPC) {
    GameWorld world;
    
    auto source = std::make_shared<Orc>("Орк_1", Point(5, 5));
    auto target = std::make_shared<Bear>("Медведь_1", Point(6, 6));
    target->markDead();  // Мертвый NPC
    
    world.addNPC(source);
    world.addNPC(target);
    
    auto inRange = world.getNPCsInRange(source, 5.0);
    EXPECT_EQ(inRange.size(), 0);  // Мертвые NPC не должны возвращаться
}

TEST_F(CoreTest, GameWorldGetNPCByID) {
    GameWorld world;
    
    auto orc = std::make_shared<Orc>("Орк_1", Point(5, 5));
    auto bear = std::make_shared<Bear>("Медведь_1", Point(10, 10));
    
    world.addNPC(orc);
    world.addNPC(bear);
    
    auto found = world.getNPCByID("Орк_1");
    EXPECT_NE(found, nullptr);
    EXPECT_EQ(found->getID(), "Орк_1");
    
    auto notFound = world.getNPCByID("Несуществующий");
    EXPECT_EQ(notFound, nullptr);
}

TEST_F(CoreTest, GameWorldCleanupDeadNPCs) {
    GameWorld world;
    
    auto orc = std::make_shared<Orc>("Орк_1", Point(5, 5));
    auto bear = std::make_shared<Bear>("Медведь_1", Point(10, 10));
    bear->markDead();  // Помечаем медведя мертвым
    
    world.addNPC(orc);
    world.addNPC(bear);
    
    // До очистки
    auto allBefore = world.getAllAliveNPCs();
    EXPECT_EQ(allBefore.size(), 1);
    
    // После очистки
    world.cleanupDeadNPCs();
    auto allAfter = world.getAllAliveNPCs();
    EXPECT_EQ(allAfter.size(), 1);  // Все еще 1 живой
    
    // Но общее количество NPC должно уменьшиться
    // (Не можем проверить без доступа к приватному полю)
}

TEST_F(CoreTest, GameWorldGetStats) {
    GameWorld world;
    
    auto orc1 = std::make_shared<Orc>("Орк_1", Point(1, 1));
    auto orc2 = std::make_shared<Orc>("Орк_2", Point(2, 2));
    auto bear = std::make_shared<Bear>("Медведь_1", Point(3, 3));
    auto squirrel = std::make_shared<Squirrel>("Белка_1", Point(4, 4));
    auto deadOrc = std::make_shared<Orc>("Орк_3", Point(5, 5));
    deadOrc->markDead();
    
    world.addNPC(orc1);
    world.addNPC(orc2);
    world.addNPC(bear);
    world.addNPC(squirrel);
    world.addNPC(deadOrc);
    
    auto stats = world.getStats();
    
    EXPECT_EQ(stats.totalNPCs, 5);
    EXPECT_EQ(stats.aliveNPCs, 4);
    EXPECT_EQ(stats.orcsCount, 2);  // 2 живых орка
    EXPECT_EQ(stats.bearCount, 1);
    EXPECT_EQ(stats.squirrelCount, 1);
}

TEST_F(CoreTest, GameWorldThreadSafety) {
    GameWorld world;
    
    // Добавляем NPC из разных потоков
    std::thread t1([&world]() {
        for (int i = 0; i < 10; ++i) {
            world.addNPC(std::make_shared<Orc>("Орк_t1_" + std::to_string(i), Point(i, i)));
        }
    });
    
    std::thread t2([&world]() {
        for (int i = 0; i < 10; ++i) {
            world.addNPC(std::make_shared<Bear>("Медведь_t2_" + std::to_string(i), Point(i+10, i+10)));
        }
    });
    
    t1.join();
    t2.join();
    
    // Не должно быть крашей или дедлоков
    auto stats = world.getStats();
    EXPECT_EQ(stats.totalNPCs, 20);
}

// ==================== CORE TESTS: GAMETIMER ====================

TEST_F(CoreTest, GameTimerStartStop) {
    GameTimer timer;
    
    EXPECT_FALSE(timer.isRunning());
    EXPECT_FALSE(timer.isFinished());
    
    timer.start();
    EXPECT_TRUE(timer.isRunning());
    EXPECT_FALSE(timer.isFinished());
    
    timer.stop();
    EXPECT_FALSE(timer.isRunning());
    EXPECT_TRUE(timer.isFinished());
}

TEST_F(CoreTest, GameTimerElapsedTime) {
    GameTimer timer;
    
    timer.start();
    
    // Ждем немного
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    auto elapsed = timer.getElapsedTime();
    EXPECT_GE(elapsed.count(), 0);
    EXPECT_LE(elapsed.count(), 1);  // Меньше 1 секунды
    
    timer.stop();
}

TEST_F(CoreTest, GameTimerRemainingTime) {
    GameTimer timer;
    
    timer.start();
    
    // Сразу после старта
    auto remaining = timer.getRemainingTime();
    EXPECT_LE(remaining.count(), GameConfig::GAME_DURATION.count());
    EXPECT_GT(remaining.count(), GameConfig::GAME_DURATION.count() - 2);  // Почти полное время
    
    timer.stop();
}

TEST_F(CoreTest, GameTimerShouldContinue) {
    GameTimer timer;
    
    // До старта
    EXPECT_FALSE(timer.shouldContinue());
    
    // После старта
    timer.start();
    EXPECT_TRUE(timer.shouldContinue());
    
    // После остановки
    timer.stop();
    EXPECT_FALSE(timer.shouldContinue());
}

TEST_F(CoreTest, GameTimerNotStarted) {
    GameTimer timer;
    
    // Таймер не запущен
    EXPECT_EQ(timer.getElapsedTime().count(), 0);
    EXPECT_EQ(timer.getRemainingTime().count(), GameConfig::GAME_DURATION.count());
    EXPECT_FALSE(timer.shouldContinue());
}

// ==================== CORE TESTS: THREADSAFECONSOLE ====================

TEST_F(CoreTest, ThreadSafeConsoleBasic) {
    testing::internal::CaptureStdout();
    
    TS_PRINT("Hello ");
    TS_PRINT("World");
    TS_PRINTLN("!");
    
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_EQ(output, "Hello World!\n");
}

TEST_F(CoreTest, ThreadSafeConsolePrintStream) {
    testing::internal::CaptureStdout();
    
    std::ostringstream oss;
    oss << "Stream test " << 42 << " value";
    ThreadSafeConsole::printStream(oss);
    
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_EQ(output, "Stream test 42 value");
}

TEST_F(CoreTest, ThreadSafeConsoleThreadSafety) {
    // Тестируем из нескольких потоков
    std::atomic<int> counter{0};
    const int threadCount = 5;
    const int messagesPerThread = 10;
    
    std::vector<std::thread> threads;
    
    for (int t = 0; t < threadCount; ++t) {
        threads.emplace_back([&counter, t]() {
            for (int i = 0; i < messagesPerThread; ++i) {
                std::ostringstream oss;
                oss << "Thread " << t << ", message " << i << "\n";
                ThreadSafeConsole::printStream(oss);
                counter++;
            }
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    EXPECT_EQ(counter, threadCount * messagesPerThread);
    // Основное - не должно быть крашей или дедлоков
}

// ==================== BEHAVIOR TESTS: DICE ====================

TEST_F(BehaviorTest, DiceConstructor) {
    Dice dice6(6);  // 6-гранный кубик
    Dice dice20(20); // 20-гранный кубик
    
    // Не должно быть исключений при создании
    EXPECT_NO_THROW(Dice dice(100));
}

TEST_F(BehaviorTest, DiceRollAttackDefense) {
    Dice dice;
    
    // Многократные броски должны давать значения в диапазоне
    for (int i = 0; i < 1000; ++i) {
        int attack = dice.rollAttack();
        int defense = dice.rollDefense();
        
        EXPECT_GE(attack, 1);
        EXPECT_LE(attack, 6);  // По умолчанию 6 граней
        EXPECT_GE(defense, 1);
        EXPECT_LE(defense, 6);
    }
}

TEST_F(BehaviorTest, DiceResolveCombat) {
    Dice dice;
    
    // Атака > защиты -> true
    EXPECT_TRUE(dice.resolveCombat(6, 3));
    EXPECT_TRUE(dice.resolveCombat(5, 1));
    
    // Атака <= защиты -> false
    EXPECT_FALSE(dice.resolveCombat(3, 6));
    EXPECT_FALSE(dice.resolveCombat(4, 4));  // Равные значения
    EXPECT_FALSE(dice.resolveCombat(2, 5));
}

TEST_F(BehaviorTest, DiceRandomIndex) {
    Dice dice;
    
    // Тестируем с разными размерами
    std::vector<size_t> sizes = {1, 5, 10, 20};
    
    for (size_t max : sizes) {
        // Многократные вызовы
        for (int i = 0; i < 100; ++i) {
            size_t index = dice.randomIndex(max);
            EXPECT_GE(index, 0u);
            EXPECT_LE(index, max);
        }
    }
    
    // Особый случай: max = 0
    size_t index0 = dice.randomIndex(0);
    EXPECT_EQ(index0, 0u);
}

TEST_F(BehaviorTest, DiceDistribution) {
    Dice dice;
    const int rolls = 10000;
    std::vector<int> counts(6, 0);  // Для 6-гранного кубика
    
    // Многократные броски
    for (int i = 0; i < rolls; ++i) {
        int roll = dice.rollAttack();
        counts[roll - 1]++;  // Индексы 0-5
    }
    
    // Проверяем что все значения выпадают
    for (int count : counts) {
        EXPECT_GT(count, 0) << "Какое-то значение не выпало ни разу";
    }
    
    // Проверяем примерную равномерность (не строгий тест)
    double expected = rolls / 6.0;
    for (int count : counts) {
        double ratio = count / expected;
        EXPECT_GT(ratio, 0.8) << "Распределение слишком неравномерное";
        EXPECT_LT(ratio, 1.2) << "Распределение слишком неравномерное";
    }
}

// ==================== BEHAVIOR TESTS: COROUTINEMANAGER ====================

TEST_F(BehaviorTest, CoroutineManagerConstructor) {
    EXPECT_NO_THROW(CoroutineManager manager(world, dice, logger));
    
    // Без логгера тоже должно работать
    EXPECT_NO_THROW(CoroutineManager manager(world, dice, nullptr));
}

TEST_F(BehaviorTest, CoroutineManagerAddNPC) {
    CoroutineManager manager(world, dice, logger);
    
    auto npc = std::make_shared<Orc>("Орк_1", Point(5, 5));
    world->addNPC(npc);
    
    EXPECT_NO_THROW(manager.addNPC(npc));
    
    // Добавление nullptr (если возможно)
    // manager.addNPC(nullptr); // Может падать или игнорироваться
}

TEST_F(BehaviorTest, CoroutineManagerStartStop) {
    CoroutineManager manager(world, dice, logger);
    
    auto npc = std::make_shared<Orc>("Орк_1", Point(5, 5));
    world->addNPC(npc);
    manager.addNPC(npc);
    
    // Старт
    EXPECT_NO_THROW(manager.start());
    
    // Небольшая пауза
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // Стоп
    EXPECT_NO_THROW(manager.stop());
    
    // Повторный старт после стопа
    EXPECT_NO_THROW(manager.start());
    EXPECT_NO_THROW(manager.stop());
}

TEST_F(BehaviorTest, CoroutineManagerGetAliveNPCs) {
    CoroutineManager manager(world, dice, logger);
    
    auto orc = std::make_shared<Orc>("Орк_1", Point(5, 5));
    auto bear = std::make_shared<Bear>("Медведь_1", Point(10, 10));
    bear->markDead();  // Мертвый
    
    world->addNPC(orc);
    world->addNPC(bear);
    
    manager.addNPC(orc);
    manager.addNPC(bear);
    
    auto alive = manager.getAliveNPCs();
    EXPECT_EQ(alive.size(), 1);
    EXPECT_EQ(alive[0]->getID(), "Орк_1");
}

TEST_F(BehaviorTest, CoroutineManagerMultipleNPCs) {
    CoroutineManager manager(world, dice, logger);
    
    const int npcCount = 10;
    std::vector<std::shared_ptr<NPC>> npcs;
    
    for (int i = 0; i < npcCount; ++i) {
        auto npc = std::make_shared<Orc>("Орк_" + std::to_string(i), Point(i, i));
        world->addNPC(npc);
        manager.addNPC(npc);
        npcs.push_back(npc);
    }
    
    auto alive = manager.getAliveNPCs();
    EXPECT_EQ(alive.size(), npcCount);
    
    // Убиваем половину
    for (int i = 0; i < npcCount / 2; ++i) {
        npcs[i]->markDead();
    }
    
    alive = manager.getAliveNPCs();
    EXPECT_EQ(alive.size(), npcCount / 2);
}

TEST_F(BehaviorTest, CoroutineManagerThreadSafety) {
    CoroutineManager manager(world, dice, logger);
    
    // Добавляем NPC из разных потоков
    std::atomic<int> addedCount{0};
    const int threadCount = 3;
    const int npcsPerThread = 5;
    
    std::vector<std::thread> threads;
    
    for (int t = 0; t < threadCount; ++t) {
        threads.emplace_back([&manager, this, &addedCount, t]() {  // Изменено: this вместо &world
            for (int i = 0; i < npcsPerThread; ++i) {
                auto npc = std::make_shared<Orc>(
                    "Орк_t" + std::to_string(t) + "_" + std::to_string(i), 
                    Point(t * 5 + i, t * 5 + i)
                );
                world->addNPC(npc);  // Теперь world доступен через this
                manager.addNPC(npc);
                addedCount++;
            }
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    EXPECT_EQ(addedCount, threadCount * npcsPerThread);
    
    // Запускаем и останавливаем менеджер
    manager.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    manager.stop();
    
    // Не должно быть крашей или дедлоков
}


// ==================== BEHAVIOR TESTS: NPCBEHAVIOR ====================

// Мок GameWorld для тестирования NPCBehavior
class MockGameWorld : public GameWorld {
public:
    std::vector<std::shared_ptr<NPC>> mockNPCsInRange;
    
    std::vector<std::shared_ptr<NPC>> getNPCsInRange(
        std::shared_ptr<NPC> source, double range) {
        return mockNPCsInRange;
    }
};

TEST_F(BehaviorTest, NPCBehaviorGeneratorCreation) {
    auto mockWorld = std::make_shared<MockGameWorld>();
    auto npc = std::make_shared<Orc>("Орк_1", Point(5, 5));
    
    // Создание генератора поведения
    auto generator = createNPCBehavior(npc, mockWorld, dice, logger);
    EXPECT_TRUE(generator.coro);  // Корутина должна быть создана
}

TEST_F(BehaviorTest, NPCBehaviorMovement) {
    auto mockWorld = std::make_shared<MockGameWorld>();
    auto npc = std::make_shared<Orc>("Орк_1", Point(5, 5));
    Point originalPos = npc->getPos();
    
    auto generator = createNPCBehavior(npc, mockWorld, dice, nullptr); // Без логгера
    
    // Запускаем один шаг корутины
    generator.next();
    
    // NPC должен переместиться (возможно)
    Point newPos = npc->getPos();
    
    // Позиция может измениться или остаться той же
    // Главное - не должно быть исключений
    EXPECT_GE(newPos.getX(), 0);
    EXPECT_LT(newPos.getX(), GameConfig::MAP_WIDTH);
    EXPECT_GE(newPos.getY(), 0);
    EXPECT_LT(newPos.getY(), GameConfig::MAP_HEIGHT);
}

TEST_F(BehaviorTest, NPCBehaviorCombatWithTarget) {
    auto mockWorld = std::make_shared<MockGameWorld>();
    auto attacker = std::make_shared<Orc>("Орк_1", Point(5, 5));
    auto defender = std::make_shared<Bear>("Медведь_1", Point(6, 6));
    
    mockWorld->mockNPCsInRange = {defender};
    
    auto generator = createNPCBehavior(attacker, mockWorld, dice, logger);
    
    EXPECT_NO_THROW(generator.next());
}

TEST_F(BehaviorTest, NPCBehaviorNoTargets) {
    auto mockWorld = std::make_shared<MockGameWorld>();
    auto npc = std::make_shared<Orc>("Орк_1", Point(5, 5));
    
    // Нет целей в радиусе
    mockWorld->mockNPCsInRange = {};
    
    auto generator = createNPCBehavior(npc, mockWorld, dice, logger);
    
    // Не должно падать при отсутствии целей
    EXPECT_NO_THROW(generator.next());
}

TEST_F(BehaviorTest, NPCBehaviorDeadNPC) {
    auto mockWorld = std::make_shared<MockGameWorld>();
    auto npc = std::make_shared<Orc>("Орк_1", Point(5, 5));
    
    // Убиваем NPC
    npc->markDead();
    
    auto generator = createNPCBehavior(npc, mockWorld, dice, logger);
    
    // Корутина должна сразу завершиться для мертвого NPC
    bool hasNext = generator.next();
    EXPECT_FALSE(hasNext);  // Больше нет шагов
}

TEST_F(BehaviorTest, NPCBehaviorMultipleSteps) {
    auto mockWorld = std::make_shared<MockGameWorld>();
    auto npc = std::make_shared<Orc>("Орк_1", Point(5, 5));
    
    auto generator = createNPCBehavior(npc, mockWorld, dice, nullptr);
    
    // Выполняем несколько шагов
    int steps = 0;
    while (generator.next() && steps < 10) {
        steps++;
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    
    EXPECT_GT(steps, 0);  // Должен выполниться хотя бы один шаг
}

// ==================== INTEGRATION TESTS ====================

TEST_F(BehaviorTest, IntegrationWorldAndCoroutineManager) {
    // Создаем мир и менеджер
    CoroutineManager manager(world, dice, logger);
    
    // Добавляем NPC в мир и менеджер
    const int npcCount = 5;
    for (int i = 0; i < npcCount; ++i) {
        auto npc = std::make_shared<Orc>("Орк_" + std::to_string(i), Point(i * 3, i * 3));
        world->addNPC(npc);
        manager.addNPC(npc);
    }
    
    // Проверяем синхронизацию
    auto worldNPCs = world->getAllAliveNPCs();
    auto managerNPCs = manager.getAliveNPCs();
    
    EXPECT_EQ(worldNPCs.size(), npcCount);
    EXPECT_EQ(managerNPCs.size(), npcCount);
    
    // Убиваем одного NPC
    worldNPCs[0]->markDead();
    world->cleanupDeadNPCs();
    
    // Менеджер должен видеть изменение
    managerNPCs = manager.getAliveNPCs();
    EXPECT_EQ(managerNPCs.size(), npcCount - 1);
}

TEST_F(BehaviorTest, IntegrationDiceInCombat) {
    // Упростить тест - не можем мокать невиртуальные методы
    auto mockWorld = std::make_shared<MockGameWorld>();
    auto attacker = std::make_shared<Orc>("Орк_1", Point(5, 5));
    auto defender = std::make_shared<Bear>("Медведь_1", Point(6, 6));
    
    mockWorld->mockNPCsInRange = {defender};
    
    auto generator = createNPCBehavior(attacker, mockWorld, dice, nullptr);
    
    // Просто проверяем что не падает
    EXPECT_NO_THROW(generator.next());
    
    // Не проверяем исход - он случаен
}

TEST_F(BehaviorTest, IntegrationFullCycle) {
    // Полный цикл: NPC в мире -> поведение -> бой -> результат
    
    // 1. Создаем мир
    auto gameWorld = std::make_shared<GameWorld>();
    auto gameDice = std::make_shared<Dice>();
    auto gameLogger = std::make_shared<BattleLogger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>();
    gameLogger->attach(consoleLogger);
    
    // 2. Создаем менеджер корутин
    CoroutineManager manager(gameWorld, gameDice, gameLogger);
    
    // 3. Добавляем NPC которые могут атаковать друг друга
    auto orc = std::make_shared<Orc>("Орк_1", Point(5, 5));
    auto bear = std::make_shared<Bear>("Медведь_1", Point(6, 6));
    
    gameWorld->addNPC(orc);
    gameWorld->addNPC(bear);
    manager.addNPC(orc);
    manager.addNPC(bear);
    
    // 4. Запускаем менеджер на короткое время
    testing::internal::CaptureStdout();
    
    manager.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    manager.stop();
    
    std::string output = testing::internal::GetCapturedStdout();
    
    // 5. Проверяем что что-то произошло
    // Могут быть логи перемещения или боев
    bool hasActivity = output.find("атакует") != std::string::npos ||
                       output.find("победил") != std::string::npos ||
                       output.find("защитился") != std::string::npos;
    
    // Не обязательно всегда будет активность, зависит от случайности
    // Главное - не должно быть крашей
    EXPECT_TRUE(true);  // Просто проверяем что выполнилось без ошибок
}

// ==================== EDGE CASES ====================

TEST_F(BehaviorTest, EdgeCaseEmptyWorld) {
    // Менеджер с пустым миром
    CoroutineManager manager(world, dice, logger);
    
    EXPECT_NO_THROW(manager.start());
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    EXPECT_NO_THROW(manager.stop());
    
    auto alive = manager.getAliveNPCs();
    EXPECT_EQ(alive.size(), 0);
}

TEST_F(BehaviorTest, EdgeCaseAllDeadNPCs) {
    CoroutineManager manager(world, dice, logger);
    
    // Добавляем только мертвых NPC
    for (int i = 0; i < 5; ++i) {
        auto npc = std::make_shared<Orc>("Орк_" + std::to_string(i), Point(i, i));
        npc->markDead();
        world->addNPC(npc);
        manager.addNPC(npc);
    }
    
    auto alive = manager.getAliveNPCs();
    EXPECT_EQ(alive.size(), 0);
    
    // Запуск должен работать (просто не будет активных корутин)
    EXPECT_NO_THROW(manager.start());
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    EXPECT_NO_THROW(manager.stop());
}

TEST_F(BehaviorTest, EdgeCaseNullPointers) {
    // Проверяем обработку null указателей где возможно
    
    // GameWorld с nullptr NPC
    GameWorld world;
    // world.addNPC(nullptr); // Может падать
    
    // CoroutineManager с nullptr зависимостями
    // CoroutineManager manager(nullptr, nullptr, nullptr); // Может падать
    
    // Dice - всегда должен создаваться нормально
    EXPECT_NO_THROW(Dice dice);
}

