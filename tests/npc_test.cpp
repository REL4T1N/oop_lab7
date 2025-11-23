#include <gtest/gtest.h>
#include <sstream>
#include "../include/npc/elf.h"
#include "../include/npc/dragon.h"
#include "../include/npc/druid.h"

// Фикстура для общих тестов
class NPCTest : public ::testing::Test {
protected:
    Point pos1{100, 100};
    Point pos2{105, 105};
    Point posFar{400, 400};
    int defaultRange = 10;
};

// Базовые тесты для всех NPC
TEST_F(NPCTest, ElfCreation) {
    Elf elf("Леголас", pos1, defaultRange);
    EXPECT_EQ(elf.getName(), "Леголас");
    EXPECT_EQ(elf.getType(), "Эльф");
    EXPECT_TRUE(elf.isAlive());
    EXPECT_EQ(elf.getPosition(), pos1);
    EXPECT_EQ(elf.getAttackRange(), defaultRange);
}

TEST_F(NPCTest, DragonCreation) {
    Dragon dragon("Смауг", pos1, defaultRange);
    EXPECT_EQ(dragon.getName(), "Смауг");
    EXPECT_EQ(dragon.getType(), "Дракон");
    EXPECT_TRUE(dragon.isAlive());
    EXPECT_EQ(dragon.getPosition(), pos1);
    EXPECT_EQ(dragon.getAttackRange(), defaultRange);
}

TEST_F(NPCTest, DruidCreation) {
    Druid druid("Мерлин", pos1, defaultRange);
    EXPECT_EQ(druid.getName(), "Мерлин");
    EXPECT_EQ(druid.getType(), "Друид");
    EXPECT_TRUE(druid.isAlive());
    EXPECT_EQ(druid.getPosition(), pos1);
    EXPECT_EQ(druid.getAttackRange(), defaultRange);
}

// Тесты состояния (жив/мертв)
TEST_F(NPCTest, MarkDead) {
    Elf elf("Эльф", pos1, defaultRange);
    EXPECT_TRUE(elf.isAlive());
    
    elf.markDead();
    EXPECT_FALSE(elf.isAlive());
}

// Тесты расстояния
TEST_F(NPCTest, DistanceCalculation) {
    Elf elf("Эльф", pos1, defaultRange);
    Dragon dragon("Дракон", pos2, defaultRange);
    
    double expectedDistance = pos1.distanceTo(pos2);
    EXPECT_DOUBLE_EQ(elf.distanceTo(&dragon), expectedDistance);
    EXPECT_DOUBLE_EQ(dragon.distanceTo(&elf), expectedDistance);
}

// ТЕСТЫ ЛОГИКИ АТАКИ ЭЛЬФА
TEST_F(NPCTest, ElfCanAttackDruid) {
    Elf elf("Эльф", pos1, 15);  // Дальность 15
    Druid druid("Друид", pos2, defaultRange);
    
    EXPECT_TRUE(elf.canAttack(&druid));  // Без параметра range
}

TEST_F(NPCTest, ElfCannotAttackDruidOutOfRange) {
    Elf elf("Эльф", pos1, 5);  // Малая дальность
    Druid druid("Друид", posFar, defaultRange);
    
    EXPECT_FALSE(elf.canAttack(&druid));  // Без параметра range
}

TEST_F(NPCTest, ElfCannotAttackDragon) {
    Elf elf("Эльф", pos1, defaultRange);
    Dragon dragon("Дракон", pos2, defaultRange);
    
    EXPECT_FALSE(elf.canAttack(&dragon));  // Неправильный тип цели
}

TEST_F(NPCTest, ElfCannotAttackElf) {
    Elf elf1("Эльф1", pos1, defaultRange);
    Elf elf2("Эльф2", pos2, defaultRange);
    
    EXPECT_FALSE(elf1.canAttack(&elf2));  // Не может атаковать своего типа
}

// ТЕСТЫ ЛОГИКИ АТАКИ ДРАКОНА
TEST_F(NPCTest, DragonCanAttackElf) {
    Dragon dragon("Дракон", pos1, 15);
    Elf elf("Эльф", pos2, defaultRange);
    
    EXPECT_TRUE(dragon.canAttack(&elf));
}

TEST_F(NPCTest, DragonCannotAttackElfOutOfRange) {
    Dragon dragon("Дракон", pos1, 5);  // Малая дальность
    Elf elf("Эльф", posFar, defaultRange);
    
    EXPECT_FALSE(dragon.canAttack(&elf));
}

TEST_F(NPCTest, DragonCannotAttackDruid) {
    Dragon dragon("Дракон", pos1, defaultRange);
    Druid druid("Друид", pos2, defaultRange);
    
    EXPECT_FALSE(dragon.canAttack(&druid));
}

TEST_F(NPCTest, DragonCannotAttackDragon) {
    Dragon dragon1("Дракон1", pos1, defaultRange);
    Dragon dragon2("Дракон2", pos2, defaultRange);
    
    EXPECT_FALSE(dragon1.canAttack(&dragon2));
}

// ТЕСТЫ ЛОГИКИ АТАКИ ДРУИДА
TEST_F(NPCTest, DruidCanAttackDragon) {
    Druid druid("Друид", pos1, 15);
    Dragon dragon("Дракон", pos2, defaultRange);
    
    EXPECT_TRUE(druid.canAttack(&dragon));
}

TEST_F(NPCTest, DruidCannotAttackDragonOutOfRange) {
    Druid druid("Друид", pos1, 5);  // Малая дальность
    Dragon dragon("Дракон", posFar, defaultRange);
    
    EXPECT_FALSE(druid.canAttack(&dragon));
}

TEST_F(NPCTest, DruidCannotAttackElf) {
    Druid druid("Друид", pos1, defaultRange);
    Elf elf("Эльф", pos2, defaultRange);
    
    EXPECT_FALSE(druid.canAttack(&elf));
}

TEST_F(NPCTest, DruidCannotAttackDruid) {
    Druid druid1("Друид1", pos1, defaultRange);
    Druid druid2("Друид2", pos2, defaultRange);
    
    EXPECT_FALSE(druid1.canAttack(&druid2));
}

// Тесты метода attack (проверяем что не падает)
TEST_F(NPCTest, AttackMethod) {
    Elf elf("Эльф", pos1, defaultRange);
    Druid druid("Друид", pos2, defaultRange);
    
    // Просто проверяем что метод выполняется без исключений
    EXPECT_NO_THROW(elf.attack(&druid));
    EXPECT_NO_THROW(druid.attack(&elf));
}

// Тесты изменения дальности атаки
TEST_F(NPCTest, SetAttackRange) {
    Elf elf("Эльф", pos1, 10);
    EXPECT_EQ(elf.getAttackRange(), 10);
    
    elf.setAttackRange(20);
    EXPECT_EQ(elf.getAttackRange(), 20);
}

// Тесты вывода (attack выводит в cout)
TEST_F(NPCTest, AttackOutput) {
    Elf elf("Леголас", pos1, defaultRange);
    Druid druid("Мерлин", pos2, defaultRange);
    
    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());
    
    elf.attack(&druid);
    
    std::cout.rdbuf(old);
    std::string output = buffer.str();
    
    EXPECT_NE(output.find("Леголас"), std::string::npos);
    EXPECT_NE(output.find("атакует"), std::string::npos);
    EXPECT_NE(output.find("Мерлин"), std::string::npos);
}

// Тесты граничных случаев
TEST_F(NPCTest, AttackDeadTarget) {
    Elf elf("Эльф", pos1, defaultRange);
    Druid druid("Друид", pos2, defaultRange);
    
    druid.markDead();
    
    // Мертвая цель не должна влиять на canAttack
    EXPECT_FALSE(elf.canAttack(&druid));
}

TEST_F(NPCTest, ZeroRange) {
    Elf elf("Эльф", pos1, 0);  // Нулевая дальность
    Druid druid("Друид", pos2, defaultRange);
    
    EXPECT_FALSE(elf.canAttack(&druid));
}

TEST_F(NPCTest, SamePosition) {
    Elf elf("Эльф", pos1, 1);  // Малая дальность
    Druid druid("Друид", pos1, defaultRange);  // Та же позиция
    
    EXPECT_TRUE(elf.canAttack(&druid));  // Даже при малой дальности
}