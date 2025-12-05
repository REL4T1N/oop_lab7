#include <gtest/gtest.h>
#include <sstream>
#include "../include/npc/Bear.h"
#include "../include/npc/Orc.h"
#include "../include/npc/Squirrel.h"
#include "../include/core/Point.h"

// Фикстура для общих тестов NPC
class NPCTest : public ::testing::Test {
protected:
    Point pos1{5, 5};
    Point pos2{8, 8};      // В радиусе атаки (расстояние ~4.24)
    Point posFar{20, 20};  // Далеко за пределами атаки
    
    void SetUp() override {
        // Можно инициализировать общие данные
    }
    
    void TearDown() override {
        // Очистка после тестов
    }
};

// ==================== БАЗОВЫЕ ТЕСТЫ NPC ====================

// Тест создания NPC
TEST_F(NPCTest, NPCCreation) {
    Bear bear("Медведь_1", pos1);
    
    EXPECT_EQ(bear.getID(), "Медведь_1");
    EXPECT_EQ(bear.getPos(), pos1);
    EXPECT_EQ(bear.getType(), "Медведь");
    EXPECT_TRUE(bear.isAlive());
    EXPECT_EQ(bear.getSymbol(), 'B');
    EXPECT_EQ(bear.getMoveRange(), GameConfig::Bear::MOVE_RANGE);
    EXPECT_EQ(bear.getAttackRange(), GameConfig::Bear::ATTACK_RANGE);
}

// Тест состояния NPC (жив/мертв)
TEST_F(NPCTest, NPCMarkDead) {
    Bear bear("Медведь_1", pos1);
    
    EXPECT_TRUE(bear.isAlive());
    bear.markDead();
    EXPECT_FALSE(bear.isAlive());
}

// Тест изменения позиции
TEST_F(NPCTest, NPCSetPosition) {
    Bear bear("Медведь_1", pos1);
    Point newPos{10, 10};
    
    bear.setPos(newPos);
    EXPECT_EQ(bear.getPos(), newPos);
}

// Тест расчета расстояния
TEST_F(NPCTest, NPCDistanceTo) {
    Bear bear1("Медведь_1", pos1);
    Bear bear2("Медведь_2", pos2);
    
    double expectedDistance = pos1.distanceTo(pos2);
    EXPECT_DOUBLE_EQ(bear1.distanceTo(std::make_shared<Bear>(bear2)), expectedDistance);
}

// Тест проверки радиуса атаки
TEST_F(NPCTest, NPCIsInAttackRange) {
    Bear bear("Медведь_1", pos1);
    Squirrel squirrel("Белка_1", pos2, 5, 5);
    Squirrel squirrelFar("Белка_2", posFar, 5, 5);
    
    // Близкая цель в радиусе
    EXPECT_TRUE(bear.isInAttackRange(std::make_shared<Squirrel>(squirrel)));
    
    // Далекая цель вне радиуса
    EXPECT_FALSE(bear.isInAttackRange(std::make_shared<Squirrel>(squirrelFar)));
    
    // Мертвая цель
    squirrel.markDead();
    EXPECT_FALSE(bear.isInAttackRange(std::make_shared<Squirrel>(squirrel)));
}

// Тест расчета следующей позиции
TEST_F(NPCTest, NPCCalculateNextPosition) {
    Bear bear("Медведь_1", Point(10, 10));
    
    // Проверяем, что позиция остается в пределах карты
    for (int i = 0; i < 100; ++i) {  // Многократно для надежности
        Point newPos = bear.calculateNextPosition();
        
        EXPECT_GE(newPos.getX(), 0);
        EXPECT_LT(newPos.getX(), GameConfig::MAP_WIDTH);
        EXPECT_GE(newPos.getY(), 0);
        EXPECT_LT(newPos.getY(), GameConfig::MAP_HEIGHT);
        
        // Проверяем, что не ушли дальше move_range
        int dx = abs(newPos.getX() - 10);
        int dy = abs(newPos.getY() - 10);
        EXPECT_LE(dx, bear.getMoveRange());
        EXPECT_LE(dy, bear.getMoveRange());
    }
}

// Тест сохранения NPC
TEST_F(NPCTest, NPCSaveToStream) {
    Bear bear("Медведь_1", Point(8, 4), 5, 10);
    std::stringstream ss;
    
    bear.save(ss);
    std::string result = ss.str();
    
    // Проверяем формат: "Медведь Медведь_1 8 4 5 10"
    EXPECT_NE(result.find("Медведь Медведь_1"), std::string::npos);
    EXPECT_NE(result.find("8 4"), std::string::npos);
    EXPECT_NE(result.find("5 10"), std::string::npos);
}

// ==================== ТЕСТЫ МЕДВЕДЯ ====================

TEST_F(NPCTest, BearCreation) {
    Bear bear("Медведь_1", pos1);
    
    EXPECT_EQ(bear.getType(), "Медведь");
    EXPECT_EQ(bear.getSymbol(), GameConfig::Bear::SYMBOL);
    EXPECT_EQ(bear.getMoveRange(), GameConfig::Bear::MOVE_RANGE);
    EXPECT_EQ(bear.getAttackRange(), GameConfig::Bear::ATTACK_RANGE);
}

// Медведь может атаковать только белок
TEST_F(NPCTest, BearCanAttackSquirrel) {
    Bear bear("Медведь_1", pos1);
    Squirrel squirrel("Белка_1", pos2);
    
    EXPECT_TRUE(bear.canAttack(std::make_shared<Squirrel>(squirrel)));
}

TEST_F(NPCTest, BearCannotAttackOrc) {
    Bear bear("Медведь_1", pos1);
    Orc orc("Орк_1", pos2);
    
    EXPECT_FALSE(bear.canAttack(std::make_shared<Orc>(orc)));
}

TEST_F(NPCTest, BearCannotAttackBear) {
    Bear bear1("Медведь_1", pos1);
    Bear bear2("Медведь_2", pos2);
    
    EXPECT_FALSE(bear1.canAttack(std::make_shared<Bear>(bear2)));
}

TEST_F(NPCTest, BearCannotAttackItself) {
    Bear bear("Медведь_1", pos1);
    
    // Медведь не должен атаковать сам себя
    EXPECT_FALSE(bear.canAttack(std::make_shared<Bear>(bear)));
}

// ==================== ТЕСТЫ ОРКА ====================

TEST_F(NPCTest, OrcCreation) {
    Orc orc("Орк_1", pos1);
    
    EXPECT_EQ(orc.getType(), "Орк");
    EXPECT_EQ(orc.getSymbol(), GameConfig::Orc::SYMBOL);
    EXPECT_EQ(orc.getMoveRange(), GameConfig::Orc::MOVE_RANGE);
    EXPECT_EQ(orc.getAttackRange(), GameConfig::Orc::ATTACK_RANGE);
}

// Орк может атаковать орков и медведей
TEST_F(NPCTest, OrcCanAttackOrc) {
    Orc orc1("Орк_1", pos1);
    Orc orc2("Орк_2", pos2);
    
    EXPECT_TRUE(orc1.canAttack(std::make_shared<Orc>(orc2)));
}

TEST_F(NPCTest, OrcCanAttackBear) {
    Orc orc("Орк_1", pos1);
    Bear bear("Медведь_1", pos2);
    
    EXPECT_TRUE(orc.canAttack(std::make_shared<Bear>(bear)));
}

TEST_F(NPCTest, OrcCannotAttackSquirrel) {
    Orc orc("Орк_1", pos1);
    Squirrel squirrel("Белка_1", pos2);
    
    EXPECT_FALSE(orc.canAttack(std::make_shared<Squirrel>(squirrel)));
}

TEST_F(NPCTest, OrcCannotAttackItself) {
    Orc orc("Орк_1", pos1);
    
    EXPECT_FALSE(orc.canAttack(std::make_shared<Orc>(orc)));
}

// ==================== ТЕСТЫ БЕЛКИ ====================

TEST_F(NPCTest, SquirrelCreation) {
    Squirrel squirrel("Белка_1", pos1);
    
    EXPECT_EQ(squirrel.getType(), "Белка");
    EXPECT_EQ(squirrel.getSymbol(), GameConfig::Squirrel::SYMBOL);
    EXPECT_EQ(squirrel.getMoveRange(), GameConfig::Squirrel::MOVE_RANGE);
    EXPECT_EQ(squirrel.getAttackRange(), GameConfig::Squirrel::ATTACK_RANGE);
}

// Белка не может атаковать никого
TEST_F(NPCTest, SquirrelCannotAttackAnyone) {
    Squirrel squirrel("Белка_1", pos1);
    Bear bear("Медведь_1", pos2);
    Orc orc("Орк_1", pos2);
    Squirrel squirrel2("Белка_2", pos2);
    
    EXPECT_FALSE(squirrel.canAttack(std::make_shared<Bear>(bear)));
    EXPECT_FALSE(squirrel.canAttack(std::make_shared<Orc>(orc)));
    EXPECT_FALSE(squirrel.canAttack(std::make_shared<Squirrel>(squirrel2)));
    EXPECT_FALSE(squirrel.canAttack(std::make_shared<Squirrel>(squirrel))); // Себя тоже
}

// ==================== ТЕСТЫ КОНСТРУКТОРА ИЗ ФАЙЛА ====================

TEST_F(NPCTest, BearCustomConstructor) {
    Bear bear("Медведь_1", Point(3, 4), 7, 12);
    
    EXPECT_EQ(bear.getID(), "Медведь_1");
    EXPECT_EQ(bear.getPos(), Point(3, 4));
    EXPECT_EQ(bear.getMoveRange(), 7);
    EXPECT_EQ(bear.getAttackRange(), 12);
    EXPECT_EQ(bear.getSymbol(), GameConfig::Bear::SYMBOL);
}

TEST_F(NPCTest, OrcCustomConstructor) {
    Orc orc("Орк_1", Point(5, 6), 8, 15);
    
    EXPECT_EQ(orc.getID(), "Орк_1");
    EXPECT_EQ(orc.getPos(), Point(5, 6));
    EXPECT_EQ(orc.getMoveRange(), 8);
    EXPECT_EQ(orc.getAttackRange(), 15);
    EXPECT_EQ(orc.getSymbol(), GameConfig::Orc::SYMBOL);
}

TEST_F(NPCTest, SquirrelCustomConstructor) {
    Squirrel squirrel("Белка_1", Point(7, 8), 10, 3);
    
    EXPECT_EQ(squirrel.getID(), "Белка_1");
    EXPECT_EQ(squirrel.getPos(), Point(7, 8));
    EXPECT_EQ(squirrel.getMoveRange(), 10);
    EXPECT_EQ(squirrel.getAttackRange(), 3);
    EXPECT_EQ(squirrel.getSymbol(), GameConfig::Squirrel::SYMBOL);
}

// ==================== ТЕСТЫ ГРАНИЧНЫХ СЛУЧАЕВ ====================

TEST_F(NPCTest, ZeroCoordinates) {
    Bear bear("Медведь_1", Point(0, 0));
    Point newPos = bear.calculateNextPosition();
    
    EXPECT_GE(newPos.getX(), 0);
    EXPECT_GE(newPos.getY(), 0);
}

TEST_F(NPCTest, MaxCoordinates) {
    Point maxPos(GameConfig::MAP_WIDTH - 1, GameConfig::MAP_HEIGHT - 1);
    Bear bear("Медведь_1", maxPos);
    Point newPos = bear.calculateNextPosition();
    
    EXPECT_LT(newPos.getX(), GameConfig::MAP_WIDTH);
    EXPECT_LT(newPos.getY(), GameConfig::MAP_HEIGHT);
}

TEST_F(NPCTest, SamePositionTarget) {
    Bear bear("Медведь_1", pos1);
    Squirrel squirrel("Белка_1", pos1);
    
    // Цель на той же позиции должна быть в радиусе атаки
    EXPECT_TRUE(bear.isInAttackRange(std::make_shared<Squirrel>(squirrel)));
    EXPECT_TRUE(bear.canAttack(std::make_shared<Squirrel>(squirrel)));
}

TEST_F(NPCTest, DeadNPCInteraction) {
    Bear bear("Медведь_1", pos1);
    Squirrel squirrel("Белка_1", pos2);
    
    // Помечаем бедку мертвой
    squirrel.markDead();
    
    // Мертвая цель не должна быть в радиусе атаки
    EXPECT_FALSE(bear.isInAttackRange(std::make_shared<Squirrel>(squirrel)));
    
    // Мертвый NPC не должен атаковать
    bear.markDead();
    EXPECT_FALSE(bear.canAttack(std::make_shared<Squirrel>(squirrel)));
}

// ==================== ТЕСТЫ СИМВОЛОВ ====================

TEST_F(NPCTest, SymbolConsistency) {
    Bear bear("Медведь_1", pos1);
    Orc orc("Орк_1", pos1);
    Squirrel squirrel("Белка_1", pos1);
    
    EXPECT_EQ(bear.getSymbol(), 'B');
    EXPECT_EQ(orc.getSymbol(), 'O');
    EXPECT_EQ(squirrel.getSymbol(), 'S');
    
    // Проверяем соответствие с GameConfig
    EXPECT_EQ(bear.getSymbol(), GameConfig::Bear::SYMBOL);
    EXPECT_EQ(orc.getSymbol(), GameConfig::Orc::SYMBOL);
    EXPECT_EQ(squirrel.getSymbol(), GameConfig::Squirrel::SYMBOL);
}

// ==================== ТЕСТЫ СЕТТЕРОВ/ГЕТТЕРОВ ====================

TEST_F(NPCTest, GetterMethods) {
    Bear bear("Test_Bear", Point(3, 7), 5, 10);
    
    EXPECT_EQ(bear.getID(), "Test_Bear");
    EXPECT_EQ(bear.getPos().getX(), 3);
    EXPECT_EQ(bear.getPos().getY(), 7);
    EXPECT_EQ(bear.getMoveRange(), 5);
    EXPECT_EQ(bear.getAttackRange(), 10);
    EXPECT_TRUE(bear.isAlive());
}

// ==================== ТЕСТЫ ПОСЕТИТЕЛЯ (Visitor) ====================

// Mock Visitor для тестирования accept()
class MockVisitor : public Visitor {
public:
    bool visitedOrc = false;
    bool visitedBear = false;
    bool visitedSquirrel = false;
    
    void visit(Orc& orc) override { visitedOrc = true; }
    void visit(Squirrel& squirrel) override { visitedSquirrel = true; }
    void visit(Bear& bear) override { visitedBear = true; }
};

TEST_F(NPCTest, VisitorPattern) {
    MockVisitor visitor;
    
    Bear bear("Медведь_1", pos1);
    Orc orc("Орк_1", pos1);
    Squirrel squirrel("Белка_1", pos1);
    
    // Проверяем, что каждый NPC правильно принимает посетителя
    bear.accept(visitor);
    EXPECT_TRUE(visitor.visitedBear);
    EXPECT_FALSE(visitor.visitedOrc);
    EXPECT_FALSE(visitor.visitedSquirrel);
    
    MockVisitor visitor2;
    orc.accept(visitor2);
    EXPECT_TRUE(visitor2.visitedOrc);
    
    MockVisitor visitor3;
    squirrel.accept(visitor3);
    EXPECT_TRUE(visitor3.visitedSquirrel);
}