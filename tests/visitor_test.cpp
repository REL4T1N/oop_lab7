#include <gtest/gtest.h>
#include <sstream>
#include "../include/visitor/battleVisitor.h"
#include "../include/npc/elf.h"
#include "../include/npc/dragon.h"
#include "../include/npc/druid.h"
#include "../include/observer/battleLogger.h"
#include "../include/observer/consoleLogger.h"

class VisitorTest : public ::testing::Test {
protected:
    Point pos1{100, 100};
    Point pos2{105, 105};
    
    std::shared_ptr<BattleLogger> battleLogger;
    
    void SetUp() override {
        battleLogger = std::make_shared<BattleLogger>();
    }
};

// Тесты боев Эльфа
TEST_F(VisitorTest, ElfAttacksDruid) {
    Elf elf("Эльф", pos1, 10);
    Druid druid("Друид", pos2, 10);
    
    BattleVisitor visitor(&elf, &druid);
    elf.accept(visitor);
    
    EXPECT_TRUE(visitor.didFightOccur());
    EXPECT_EQ(visitor.getWinner(), &elf);
    EXPECT_EQ(visitor.getLoser(), &druid);
}

TEST_F(VisitorTest, ElfCannotAttackDragon) {
    Elf elf("Эльф", pos1, 10);
    Dragon dragon("Дракон", pos2, 10);
    
    BattleVisitor visitor(&elf, &dragon);
    elf.accept(visitor);
    
    EXPECT_FALSE(visitor.didFightOccur());
    EXPECT_EQ(visitor.getWinner(), nullptr);
    EXPECT_EQ(visitor.getLoser(), nullptr);
}

TEST_F(VisitorTest, ElfCannotAttackElf) {
    Elf elf1("Эльф1", pos1, 10);
    Elf elf2("Эльф2", pos2, 10);
    
    BattleVisitor visitor(&elf1, &elf2);
    elf1.accept(visitor);
    
    EXPECT_FALSE(visitor.didFightOccur());
}

// Тесты боев Дракона
TEST_F(VisitorTest, DragonAttacksElf) {
    Dragon dragon("Дракон", pos1, 10);
    Elf elf("Эльф", pos2, 10);
    
    BattleVisitor visitor(&dragon, &elf);
    dragon.accept(visitor);
    
    EXPECT_TRUE(visitor.didFightOccur());
    EXPECT_EQ(visitor.getWinner(), &dragon);
    EXPECT_EQ(visitor.getLoser(), &elf);
}

TEST_F(VisitorTest, DragonCannotAttackDruid) {
    Dragon dragon("Дракон", pos1, 10);
    Druid druid("Друид", pos2, 10);
    
    BattleVisitor visitor(&dragon, &druid);
    dragon.accept(visitor);
    
    EXPECT_FALSE(visitor.didFightOccur());
}

TEST_F(VisitorTest, DragonCannotAttackDragon) {
    Dragon dragon1("Дракон1", pos1, 10);
    Dragon dragon2("Дракон2", pos2, 10);
    
    BattleVisitor visitor(&dragon1, &dragon2);
    dragon1.accept(visitor);
    
    EXPECT_FALSE(visitor.didFightOccur());
}

// Тесты боев Друида
TEST_F(VisitorTest, DruidAttacksDragon) {
    Druid druid("Друид", pos1, 10);
    Dragon dragon("Дракон", pos2, 10);
    
    BattleVisitor visitor(&druid, &dragon);
    druid.accept(visitor);
    
    EXPECT_TRUE(visitor.didFightOccur());
    EXPECT_EQ(visitor.getWinner(), &druid);
    EXPECT_EQ(visitor.getLoser(), &dragon);
}

TEST_F(VisitorTest, DruidCannotAttackElf) {
    Druid druid("Друид", pos1, 10);
    Elf elf("Эльф", pos2, 10);
    
    BattleVisitor visitor(&druid, &elf);
    druid.accept(visitor);
    
    EXPECT_FALSE(visitor.didFightOccur());
}

TEST_F(VisitorTest, DruidCannotAttackDruid) {
    Druid druid1("Друид1", pos1, 10);
    Druid druid2("Друид2", pos2, 10);
    
    BattleVisitor visitor(&druid1, &druid2);
    druid1.accept(visitor);
    
    EXPECT_FALSE(visitor.didFightOccur());
}

// Тесты обратных комбинаций (не должны работать)
TEST_F(VisitorTest, ReverseCombinationsDoNotWork) {
    // Дракон атакует Друида - не должно работать
    Dragon dragon("Дракон", pos1, 10);
    Druid druid("Друид", pos2, 10);
    
    BattleVisitor visitor1(&dragon, &druid);
    dragon.accept(visitor1);
    EXPECT_FALSE(visitor1.didFightOccur());
    
    // Друид атакует Эльфа - не должно работать
    Druid druid2("Друид", pos1, 10);
    Elf elf("Эльф", pos2, 10);
    
    BattleVisitor visitor2(&druid2, &elf);
    druid2.accept(visitor2);
    EXPECT_FALSE(visitor2.didFightOccur());
    
    // Эльф атакует Дракона - не должно работать
    Elf elf2("Эльф", pos1, 10);
    Dragon dragon2("Дракон", pos2, 10);
    
    BattleVisitor visitor3(&elf2, &dragon2);
    elf2.accept(visitor3);
    EXPECT_FALSE(visitor3.didFightOccur());
}

// Тесты с логгером
TEST_F(VisitorTest, BattleWithLogger) {
    battleLogger->attach(std::make_shared<ConsoleLogger>());
    
    Elf elf("Эльф", pos1, 10);
    Druid druid("Друид", pos2, 10);
    
    // Перехватываем вывод
    testing::internal::CaptureStdout();
    
    BattleVisitor visitor(&elf, &druid, battleLogger);
    elf.accept(visitor);
    
    std::string output = testing::internal::GetCapturedStdout();
    
    EXPECT_TRUE(visitor.didFightOccur());
    EXPECT_NE(output.find("Эльф"), std::string::npos);
    EXPECT_NE(output.find("Друид"), std::string::npos);
}

TEST_F(VisitorTest, BattleWithoutLogger) {
    Elf elf("Эльф", pos1, 10);
    Druid druid("Друид", pos2, 10);
    
    // Перехватываем вывод
    testing::internal::CaptureStdout();
    
    BattleVisitor visitor(&elf, &druid, nullptr); // Без логгера
    elf.accept(visitor);
    
    std::string output = testing::internal::GetCapturedStdout();
    
    EXPECT_TRUE(visitor.didFightOccur());
    // Должен использоваться стандартный вывод
    EXPECT_NE(output.find("победил"), std::string::npos);
}

// Тесты состояния после боя
TEST_F(VisitorTest, BattleStateAfterFight) {
    Elf elf("Эльф", pos1, 10);
    Druid druid("Друид", pos2, 10);
    
    BattleVisitor visitor(&elf, &druid);
    
    // До боя оба живы
    EXPECT_TRUE(elf.isAlive());
    EXPECT_TRUE(druid.isAlive());
    
    elf.accept(visitor);
    
    // После боя победитель и проигравший определены
    EXPECT_TRUE(visitor.didFightOccur());
    EXPECT_EQ(visitor.getWinner()->getName(), "Эльф");
    EXPECT_EQ(visitor.getLoser()->getName(), "Друид");
    
    // Но состояние isAlive не меняется автоматически
    EXPECT_TRUE(elf.isAlive());
    EXPECT_TRUE(druid.isAlive());
}

// Тесты с мертвыми NPC
TEST_F(VisitorTest, NoFightWithDeadAttacker) {
    Elf elf("Эльф", pos1, 10);
    Druid druid("Друид", pos2, 10);
    
    elf.markDead(); // Атакующий мертв
    
    BattleVisitor visitor(&elf, &druid);
    elf.accept(visitor);
    
    // Бой не должен происходить с мертвым атакующим
    EXPECT_FALSE(visitor.didFightOccur());
}

TEST_F(VisitorTest, NoFightWithDeadDefender) {
    Elf elf("Эльф", pos1, 10);
    Druid druid("Друид", pos2, 10);
    
    druid.markDead(); // Защитник мертв
    
    BattleVisitor visitor(&elf, &druid);
    elf.accept(visitor);
    
    // Бой не должен происходить с мертвым защитником
    EXPECT_FALSE(visitor.didFightOccur());
}

// Тесты двойной диспетчеризации
TEST_F(VisitorTest, DoubleDispatchWorks) {
    // Проверяем что вызывается правильный visit метод
    Elf elf("Эльф", pos1, 10);
    Druid druid("Друид", pos2, 10);
    
    BattleVisitor visitor(&elf, &druid);
    
    // Эльф вызывает visit(Elf&), который затем вызывает visit(Druid&)
    elf.accept(visitor);
    
    EXPECT_TRUE(visitor.didFightOccur());
}

// Тесты всех возможных комбинаций атакующих/защитников
TEST_F(VisitorTest, AllAttackCombinations) {
    struct TestCase {
        std::string attackerType;
        std::string defenderType;
        bool shouldFight;
    };
    
    std::vector<TestCase> testCases = {
        {"Эльф", "Друид", true},     // Эльф vs Друид - БОЙ
        {"Дракон", "Эльф", true},    // Дракон vs Эльф - БОЙ
        {"Друид", "Дракон", true},   // Друид vs Дракон - БОЙ
        {"Эльф", "Дракон", false},   // Остальные - НЕТ БОЯ
        {"Эльф", "Эльф", false},
        {"Дракон", "Друид", false},
        {"Дракон", "Дракон", false},
        {"Друид", "Эльф", false},
        {"Друид", "Друид", false}
    };
    
    for (const auto& testCase : testCases) {
        // Создаем NPC
        std::unique_ptr<INPC> attacker;
        std::unique_ptr<INPC> defender;
        
        if (testCase.attackerType == "Эльф") attacker = std::make_unique<Elf>("Атакующий", pos1, 10);
        else if (testCase.attackerType == "Дракон") attacker = std::make_unique<Dragon>("Атакующий", pos1, 10);
        else if (testCase.attackerType == "Друид") attacker = std::make_unique<Druid>("Атакующий", pos1, 10);
        
        if (testCase.defenderType == "Эльф") defender = std::make_unique<Elf>("Защитник", pos2, 10);
        else if (testCase.defenderType == "Дракон") defender = std::make_unique<Dragon>("Защитник", pos2, 10);
        else if (testCase.defenderType == "Друид") defender = std::make_unique<Druid>("Защитник", pos2, 10);
        
        BattleVisitor visitor(attacker.get(), defender.get());
        attacker->accept(visitor);
        
        EXPECT_EQ(visitor.didFightOccur(), testCase.shouldFight)
            << "Attack: " << testCase.attackerType 
            << " vs Defense: " << testCase.defenderType
            << " should " << (testCase.shouldFight ? "FIGHT" : "NOT fight");
    }
}

// Тесты методов доступа
TEST_F(VisitorTest, AccessMethods) {
    Elf elf("Эльф", pos1, 10);
    Druid druid("Друид", pos2, 10);
    
    BattleVisitor visitor(&elf, &druid);
    
    // До боя
    EXPECT_FALSE(visitor.didFightOccur());
    EXPECT_EQ(visitor.getWinner(), nullptr);
    EXPECT_EQ(visitor.getLoser(), nullptr);
    
    elf.accept(visitor);
    
    // После боя
    EXPECT_TRUE(visitor.didFightOccur());
    EXPECT_NE(visitor.getWinner(), nullptr);
    EXPECT_NE(visitor.getLoser(), nullptr);
    EXPECT_EQ(visitor.getWinner()->getName(), "Эльф");
    EXPECT_EQ(visitor.getLoser()->getName(), "Друид");
}