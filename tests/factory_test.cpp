#include <gtest/gtest.h>
#include <fstream>
#include <sstream>
#include "../include/factory/NPCFactory.h"
#include "../include/npc/elf.h"
#include "../include/npc/dragon.h"
#include "../include/npc/druid.h"

class FactoryTest : public ::testing::Test {
protected:
    NPCFactory factory;
    int defaultRange = 10;
    
    void SetUp() override {
        // Создаем временные файлы для тестов (новый формат с range)
        std::ofstream file("../data/test_dungeon.txt");
        file << "Леголас Эльф 100 200 " << defaultRange << "\n";
        file << "Смауг Дракон 300 400 " << defaultRange << "\n";
        file << "Мерлин Друид 50 150 " << defaultRange << "\n";
        file.close();
        
        std::ofstream badFile("../data/test_bad_dungeon.txt");
        badFile << "Неправильный Неизвестный 100 200 " << defaultRange << "\n";
        badFile << "Леголас Эльф 100 200 " << defaultRange << "\n";
        badFile.close();
        
        // Файл со старым форматом (без range) для теста обратной совместимости
        std::ofstream oldFormatFile("../data/test_old_format.txt");
        oldFormatFile << "СтарыйЭльф Эльф 100 200\n";
        oldFormatFile << "СтарыйДракон Дракон 300 400\n";
        oldFormatFile.close();
        
        std::ofstream emptyFile("../data/test_empty_dungeon.txt");
        emptyFile.close();
    }
    
    void TearDown() override {
        // Удаляем временные файлы
        std::remove("../data/test_dungeon.txt");
        std::remove("../data/test_bad_dungeon.txt");
        std::remove("../data/test_old_format.txt");
        std::remove("../data/test_empty_dungeon.txt");
        std::remove("../data/test_output_dungeon.txt");
        std::remove("../data/test_roundtrip.txt");
    }
};

// Тесты создания NPC через фабрику
TEST_F(FactoryTest, CreateElf) {
    auto elf = factory.createNPC("Леголас", "Эльф", 100, 200, defaultRange);
    
    EXPECT_NE(elf, nullptr);
    EXPECT_EQ(elf->getName(), "Леголас");
    EXPECT_EQ(elf->getType(), "Эльф");
    EXPECT_EQ(elf->getAttackRange(), defaultRange);
    EXPECT_TRUE(dynamic_cast<Elf*>(elf.get()) != nullptr);
}

TEST_F(FactoryTest, CreateDragon) {
    auto dragon = factory.createNPC("Смауг", "Дракон", 300, 400, defaultRange);
    
    EXPECT_NE(dragon, nullptr);
    EXPECT_EQ(dragon->getName(), "Смауг");
    EXPECT_EQ(dragon->getType(), "Дракон");
    EXPECT_EQ(dragon->getAttackRange(), defaultRange);
    EXPECT_TRUE(dynamic_cast<Dragon*>(dragon.get()) != nullptr);
}

TEST_F(FactoryTest, CreateDruid) {
    auto druid = factory.createNPC("Мерлин", "Друид", 50, 150, defaultRange);
    
    EXPECT_NE(druid, nullptr);
    EXPECT_EQ(druid->getName(), "Мерлин");
    EXPECT_EQ(druid->getType(), "Друид");
    EXPECT_EQ(druid->getAttackRange(), defaultRange);
    EXPECT_TRUE(dynamic_cast<Druid*>(druid.get()) != nullptr);
}

TEST_F(FactoryTest, CreateUnknownType) {
    EXPECT_THROW(factory.createNPC("Неизвестный", "Орк", 100, 200, defaultRange), std::invalid_argument);
}

TEST_F(FactoryTest, CreateWithDifferentRanges) {
    // Тестируем создание с разной дальностью атаки
    auto shortRangeElf = factory.createNPC("Эльф-ближний", "Эльф", 100, 200, 5);
    auto longRangeElf = factory.createNPC("Эльф-дальний", "Эльф", 150, 250, 20);
    
    EXPECT_EQ(shortRangeElf->getAttackRange(), 5);
    EXPECT_EQ(longRangeElf->getAttackRange(), 20);
}

// Тесты валидации координат через фабрику
TEST_F(FactoryTest, CoordinateValidation) {
    // Валидные координаты
    EXPECT_NO_THROW(factory.createNPC("Эльф", "Эльф", 0, 0, defaultRange));
    EXPECT_NO_THROW(factory.createNPC("Эльф", "Эльф", 500, 500, defaultRange));
    EXPECT_NO_THROW(factory.createNPC("Эльф", "Эльф", 250, 250, defaultRange));
    
    // Невалидные координаты (должны выбрасывать исключение из Point)
    EXPECT_THROW(factory.createNPC("Эльф", "Эльф", -1, 100, defaultRange), std::out_of_range);
    EXPECT_THROW(factory.createNPC("Эльф", "Эльф", 100, -1, defaultRange), std::out_of_range);
    EXPECT_THROW(factory.createNPC("Эльф", "Эльф", 600, 100, defaultRange), std::out_of_range);
    EXPECT_THROW(factory.createNPC("Эльф", "Эльф", 100, 600, defaultRange), std::out_of_range);
}

// Тесты загрузки из файла
TEST_F(FactoryTest, LoadFromFile) {
    auto npcs = factory.loadFromFile("../data/test_dungeon.txt");
    
    EXPECT_EQ(npcs.size(), 3);
    
    // Проверяем первого NPC
    EXPECT_EQ(npcs[0]->getName(), "Леголас");
    EXPECT_EQ(npcs[0]->getType(), "Эльф");
    EXPECT_EQ(npcs[0]->getPosition().getX(), 100);
    EXPECT_EQ(npcs[0]->getPosition().getY(), 200);
    EXPECT_EQ(npcs[0]->getAttackRange(), defaultRange);
    
    // Проверяем второго NPC
    EXPECT_EQ(npcs[1]->getName(), "Смауг");
    EXPECT_EQ(npcs[1]->getType(), "Дракон");
    EXPECT_EQ(npcs[1]->getPosition().getX(), 300);
    EXPECT_EQ(npcs[1]->getPosition().getY(), 400);
    EXPECT_EQ(npcs[1]->getAttackRange(), defaultRange);
    
    // Проверяем третьего NPC
    EXPECT_EQ(npcs[2]->getName(), "Мерлин");
    EXPECT_EQ(npcs[2]->getType(), "Друид");
    EXPECT_EQ(npcs[2]->getPosition().getX(), 50);
    EXPECT_EQ(npcs[2]->getPosition().getY(), 150);
    EXPECT_EQ(npcs[2]->getAttackRange(), defaultRange);
}

TEST_F(FactoryTest, LoadFromFileWithErrors) {
    // Файл содержит одну правильную и одну неправильную запись
    testing::internal::CaptureStderr(); // Перехватываем stderr
    
    auto npcs = factory.loadFromFile("../data/test_bad_dungeon.txt");
    
    std::string output = testing::internal::GetCapturedStderr();
    
    // Должна загрузиться только одна корректная запись
    EXPECT_EQ(npcs.size(), 1);
    EXPECT_EQ(npcs[0]->getName(), "Леголас");
    EXPECT_EQ(npcs[0]->getType(), "Эльф");
    EXPECT_EQ(npcs[0]->getAttackRange(), defaultRange);
    
    // Должно быть сообщение об ошибке в stderr
    EXPECT_NE(output.find("Ошибка создания NPC"), std::string::npos);
}

TEST_F(FactoryTest, LoadFromEmptyFile) {
    auto npcs = factory.loadFromFile("../data/test_empty_dungeon.txt");
    EXPECT_EQ(npcs.size(), 0);
}

TEST_F(FactoryTest, LoadFromNonExistentFile) {
    EXPECT_THROW(factory.loadFromFile("../data/test_nonexistent.txt"), std::runtime_error);
}

// Тесты сохранения в файл
TEST_F(FactoryTest, SaveToFile) {
    std::vector<std::unique_ptr<INPC>> npcs;
    npcs.push_back(factory.createNPC("Леголас", "Эльф", 100, 200, 15));
    npcs.push_back(factory.createNPC("Смауг", "Дракон", 300, 400, 20));
    npcs.push_back(factory.createNPC("Мерлин", "Друид", 50, 150, 10));
    
    // Помечаем одного NPC мертвым
    npcs[1]->markDead();
    
    factory.saveToFile("../data/test_output_dungeon.txt", npcs);
    
    // Проверяем содержимое файла
    std::ifstream file("../data/test_output_dungeon.txt");
    std::string line;
    std::vector<std::string> lines;
    
    while (std::getline(file, line)) {
        lines.push_back(line);
    }
    
    // Должны сохраниться только живые NPC (2 из 3)
    EXPECT_EQ(lines.size(), 2);
    
    // Проверяем формат первой строки (с range)
    EXPECT_NE(lines[0].find("Леголас"), std::string::npos);
    EXPECT_NE(lines[0].find("Эльф"), std::string::npos);
    EXPECT_NE(lines[0].find("100 200 15"), std::string::npos);
    
    // Проверяем формат второй строки (с range)
    EXPECT_NE(lines[1].find("Мерлин"), std::string::npos);
    EXPECT_NE(lines[1].find("Друид"), std::string::npos);
    EXPECT_NE(lines[1].find("50 150 10"), std::string::npos);
    
    // Мертвый дракон не должен сохраниться
    bool dragonFound = false;
    for (const auto& line : lines) {
        if (line.find("Смауг") != std::string::npos) {
            dragonFound = true;
            break;
        }
    }
    EXPECT_FALSE(dragonFound);
}

TEST_F(FactoryTest, SaveToFileInvalidPath) {
    std::vector<std::unique_ptr<INPC>> npcs;
    npcs.push_back(factory.createNPC("Тест", "Эльф", 100, 200, defaultRange));
    
    // Попытка сохранить в невалидный путь
    EXPECT_THROW(factory.saveToFile("/invalid/path/test_dungeon.txt", npcs), std::runtime_error);
}

// Тесты круг trip (сохранение -> загрузка)
TEST_F(FactoryTest, RoundTrip) {
    // Создаем NPC с разной дальностью атаки
    std::vector<std::unique_ptr<INPC>> originalNpcs;
    originalNpcs.push_back(factory.createNPC("Леголас", "Эльф", 100, 200, 15));
    originalNpcs.push_back(factory.createNPC("Смауг", "Дракон", 300, 400, 25));
    
    // Сохраняем
    factory.saveToFile("../data/test_roundtrip.txt", originalNpcs);
    
    // Загружаем
    auto loadedNpcs = factory.loadFromFile("../data/test_roundtrip.txt");
    
    // Проверяем что загрузилось столько же NPC
    EXPECT_EQ(loadedNpcs.size(), originalNpcs.size());
    
    // Проверяем данные первого NPC
    EXPECT_EQ(loadedNpcs[0]->getName(), "Леголас");
    EXPECT_EQ(loadedNpcs[0]->getType(), "Эльф");
    EXPECT_EQ(loadedNpcs[0]->getPosition().getX(), 100);
    EXPECT_EQ(loadedNpcs[0]->getPosition().getY(), 200);
    EXPECT_EQ(loadedNpcs[0]->getAttackRange(), 15);
    
    // Проверяем данные второго NPC
    EXPECT_EQ(loadedNpcs[1]->getName(), "Смауг");
    EXPECT_EQ(loadedNpcs[1]->getType(), "Дракон");
    EXPECT_EQ(loadedNpcs[1]->getPosition().getX(), 300);
    EXPECT_EQ(loadedNpcs[1]->getPosition().getY(), 400);
    EXPECT_EQ(loadedNpcs[1]->getAttackRange(), 25);
    
    // Убираем временный файл
    std::remove("../data/test_roundtrip.txt");
}

// Тесты Creator классов напрямую
TEST_F(FactoryTest, ElfCreatorDirect) {
    ElfCreator creator;
    auto elf = creator.createNPC("Леголас", 100, 200, defaultRange);
    
    EXPECT_NE(elf, nullptr);
    EXPECT_EQ(elf->getName(), "Леголас");
    EXPECT_EQ(elf->getType(), "Эльф");
    EXPECT_EQ(elf->getAttackRange(), defaultRange);
    EXPECT_TRUE(dynamic_cast<Elf*>(elf.get()) != nullptr);
}

TEST_F(FactoryTest, DragonCreatorDirect) {
    DragonCreator creator;
    auto dragon = creator.createNPC("Смауг", 300, 400, defaultRange);
    
    EXPECT_NE(dragon, nullptr);
    EXPECT_EQ(dragon->getName(), "Смауг");
    EXPECT_EQ(dragon->getType(), "Дракон");
    EXPECT_EQ(dragon->getAttackRange(), defaultRange);
    EXPECT_TRUE(dynamic_cast<Dragon*>(dragon.get()) != nullptr);
}

TEST_F(FactoryTest, DruidCreatorDirect) {
    DruidCreator creator;
    auto druid = creator.createNPC("Мерлин", 50, 150, defaultRange);
    
    EXPECT_NE(druid, nullptr);
    EXPECT_EQ(druid->getName(), "Мерлин");
    EXPECT_EQ(druid->getType(), "Друид");
    EXPECT_EQ(druid->getAttackRange(), defaultRange);
    EXPECT_TRUE(dynamic_cast<Druid*>(druid.get()) != nullptr);
}

// Тесты граничных значений range
TEST_F(FactoryTest, RangeBoundaryValues) {
    // Нулевая дальность
    auto zeroRange = factory.createNPC("Эльф-нулевой", "Эльф", 100, 200, 0);
    EXPECT_EQ(zeroRange->getAttackRange(), 0);
    
    // Очень большая дальность
    auto largeRange = factory.createNPC("Эльф-дальний", "Эльф", 100, 200, 1000);
    EXPECT_EQ(largeRange->getAttackRange(), 1000);
    
    // Отрицательная дальность (должна обрабатываться, но логика боя может не работать)
    auto negativeRange = factory.createNPC("Эльф-отрицательный", "Эльф", 100, 200, -5);
    EXPECT_EQ(negativeRange->getAttackRange(), -5);
}

// Тест изменения дальности атаки после создания
TEST_F(FactoryTest, ChangeAttackRangeAfterCreation) {
    auto elf = factory.createNPC("Эльф", "Эльф", 100, 200, 10);
    EXPECT_EQ(elf->getAttackRange(), 10);
    
    // Меняем дальность атаки
    elf->setAttackRange(20);
    EXPECT_EQ(elf->getAttackRange(), 20);
    
    // Проверяем что изменение сохраняется
    elf->setAttackRange(15);
    EXPECT_EQ(elf->getAttackRange(), 15);
}