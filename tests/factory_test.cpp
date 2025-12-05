#include <gtest/gtest.h>
#include <fstream>
#include <sstream>
#include <memory>
#include "../include/factory/NPCFactory.h"
#include "../include/factory/BearCreator.h"
#include "../include/factory/OrcCreator.h"
#include "../include/factory/SquirrelCreator.h"
#include "../include/npc/Bear.h"
#include "../include/npc/Orc.h"
#include "../include/npc/Squirrel.h"
#include "../include/core/Point.h"

// Фикстура для тестов фабрики
class FactoryTest : public ::testing::Test {
protected:
    NPCFactory factory;
    
    void SetUp() override {
        // Создаем временные файлы для тестов
        std::ofstream file("../data/test_save.txt");
        file << "Медведь Медведь_1 8 4 5 10\n";
        file << "Орк Орк_2 0 6 3 8\n";
        file << "Белка Белка_3 5 7 4 2\n";
        file.close();
        
        // Файл с ошибками
        std::ofstream badFile("../data/test_bad_save.txt");
        badFile << "Медведь Медведь_1 8 4 5 10\n";
        badFile << "Неизвестный Призрак_1 1 2 3 4\n"; // Неизвестный тип
        badFile << "Орк Орк_2 0 6 3 8\n";
        badFile.close();
        
        // Пустой файл
        std::ofstream emptyFile("../data/test_empty.txt");
        emptyFile.close();
        
        // Файл с некорректным форматом
        std::ofstream wrongFormatFile("../data/test_wrong_format.txt");
        wrongFormatFile << "Медведь Медведь_1 8 4 5\n"; // Не хватает attack_range
        wrongFormatFile << "Орк Орк_2 0 6 3 8 9 10\n";  // Слишком много параметров
        wrongFormatFile.close();
    }
    
    void TearDown() override {
        // Удаляем временные файлы
        std::remove("../data/test_save.txt");
        std::remove("../data/test_bad_save.txt");
        std::remove("../data/test_empty.txt");
        std::remove("../data/test_wrong_format.txt");
        std::remove("../data/test_output.txt");
        std::remove("../data/test_roundtrip.txt");
    }
};

// ==================== ТЕСТЫ NPCFACTORY ====================

// Тест конструктора фабрики
TEST_F(FactoryTest, FactoryConstructor) {
    // Фабрика должна быть создана без ошибок
    EXPECT_NO_THROW(NPCFactory factory);
}

// Тест создания случайного NPC
TEST_F(FactoryTest, CreateRandomNPC) {
    // Создаем несколько случайных NPC и проверяем их типы
    std::set<std::string> types;
    
    for (int i = 0; i < 100; ++i) {
        auto npc = factory.createRandomNPC();
        EXPECT_NE(npc, nullptr);
        EXPECT_TRUE(npc->isAlive());
        
        types.insert(npc->getType());
    }
    
    // Проверяем, что были созданы все типы NPC
    EXPECT_TRUE(types.count("Орк") > 0);
    EXPECT_TRUE(types.count("Медведь") > 0);
    EXPECT_TRUE(types.count("Белка") > 0);
}

// Тест создания NPC по типу
TEST_F(FactoryTest, CreateNPCByType) {
    // Создаем каждого типа NPC
    auto orc = factory.createNPC("Орк");
    auto bear = factory.createNPC("Медведь");
    auto squirrel = factory.createNPC("Белка");
    
    EXPECT_NE(orc, nullptr);
    EXPECT_NE(bear, nullptr);
    EXPECT_NE(squirrel, nullptr);
    
    EXPECT_EQ(orc->getType(), "Орк");
    EXPECT_EQ(bear->getType(), "Медведь");
    EXPECT_EQ(squirrel->getType(), "Белка");
    
    // Проверяем, что создались правильные объекты
    EXPECT_TRUE(dynamic_cast<Orc*>(orc.get()) != nullptr);
    EXPECT_TRUE(dynamic_cast<Bear*>(bear.get()) != nullptr);
    EXPECT_TRUE(dynamic_cast<Squirrel*>(squirrel.get()) != nullptr);
}

// Тест создания NPC по типу с позицией
TEST_F(FactoryTest, CreateNPCByTypeWithPosition) {
    Point testPos(10, 15);
    
    auto orc = factory.createNPC("Орк", testPos);
    auto bear = factory.createNPC("Медведь", testPos);
    auto squirrel = factory.createNPC("Белка", testPos);
    
    EXPECT_NE(orc, nullptr);
    EXPECT_NE(bear, nullptr);
    EXPECT_NE(squirrel, nullptr);
    
    EXPECT_EQ(orc->getPos(), testPos);
    EXPECT_EQ(bear->getPos(), testPos);
    EXPECT_EQ(squirrel->getPos(), testPos);
}

// Тест создания NPC с неизвестным типом
TEST_F(FactoryTest, CreateNPCUnknownType) {
    EXPECT_THROW(factory.createNPC("Призрак"), std::invalid_argument);
    EXPECT_THROW(factory.createNPC(""), std::invalid_argument);
    EXPECT_THROW(factory.createNPC("Дракон"), std::invalid_argument);
}

// Тест генерации ID
TEST_F(FactoryTest, NPCIDGeneration) {
    // Создаем несколько NPC одного типа и проверяем ID
    std::set<std::string> ids;
    
    for (int i = 0; i < 5; ++i) {
        auto npc = factory.createNPC("Орк");
        ids.insert(npc->getID());
    }
    
    // Все ID должны быть уникальными
    EXPECT_EQ(ids.size(), 5);
    
    // ID должны соответствовать шаблону "Тип_номер"
    for (const auto& id : ids) {
        EXPECT_NE(id.find("Орк_"), std::string::npos);
    }
}

// ==================== ТЕСТЫ ЗАГРУЗКИ ИЗ ФАЙЛА ====================

TEST_F(FactoryTest, LoadFromFile) {
    auto npcs = factory.loadFromFile("../data/test_save.txt");
    
    EXPECT_EQ(npcs.size(), 3);
    
    // Проверяем первого NPC (Медведь)
    EXPECT_EQ(npcs[0]->getType(), "Медведь");
    EXPECT_EQ(npcs[0]->getID(), "Медведь_1");
    EXPECT_EQ(npcs[0]->getPos().getX(), 8);
    EXPECT_EQ(npcs[0]->getPos().getY(), 4);
    EXPECT_EQ(npcs[0]->getMoveRange(), 5);
    EXPECT_EQ(npcs[0]->getAttackRange(), 10);
    EXPECT_TRUE(dynamic_cast<Bear*>(npcs[0].get()) != nullptr);
    
    // Проверяем второго NPC (Орк)
    EXPECT_EQ(npcs[1]->getType(), "Орк");
    EXPECT_EQ(npcs[1]->getID(), "Орк_2");
    EXPECT_EQ(npcs[1]->getPos().getX(), 0);
    EXPECT_EQ(npcs[1]->getPos().getY(), 6);
    EXPECT_EQ(npcs[1]->getMoveRange(), 3);
    EXPECT_EQ(npcs[1]->getAttackRange(), 8);
    EXPECT_TRUE(dynamic_cast<Orc*>(npcs[1].get()) != nullptr);
    
    // Проверяем третьего NPC (Белка)
    EXPECT_EQ(npcs[2]->getType(), "Белка");
    EXPECT_EQ(npcs[2]->getID(), "Белка_3");
    EXPECT_EQ(npcs[2]->getPos().getX(), 5);
    EXPECT_EQ(npcs[2]->getPos().getY(), 7);
    EXPECT_EQ(npcs[2]->getMoveRange(), 4);
    EXPECT_EQ(npcs[2]->getAttackRange(), 2);
    EXPECT_TRUE(dynamic_cast<Squirrel*>(npcs[2].get()) != nullptr);
}

TEST_F(FactoryTest, LoadFromFileWithErrors) {
    // Файл содержит одну ошибку (неизвестный тип)
    testing::internal::CaptureStdout(); // Перехватываем TS_PRINTLN
    
    auto npcs = factory.loadFromFile("../data/test_bad_save.txt");
    
    std::string output = testing::internal::GetCapturedStdout();
    
    // Должны загрузиться только корректные записи (2 из 3)
    EXPECT_EQ(npcs.size(), 2);
    
    // Должно быть сообщение об ошибке
    EXPECT_NE(output.find("Ошибка загрузки NPC"), std::string::npos);
}

TEST_F(FactoryTest, LoadFromEmptyFile) {
    auto npcs = factory.loadFromFile("../data/test_empty.txt");
    EXPECT_EQ(npcs.size(), 0);
}

TEST_F(FactoryTest, LoadFromNonexistentFile) {
    EXPECT_THROW(factory.loadFromFile("../data/nonexistent_file.txt"), std::runtime_error);
}

TEST_F(FactoryTest, LoadFromFileWrongFormat) {
    // Файл с некорректным форматом - должен пропускать неправильные строки
    testing::internal::CaptureStdout();
    
    auto npcs = factory.loadFromFile("../data/test_wrong_format.txt");
    
    std::string output = testing::internal::GetCapturedStdout();
    
    EXPECT_EQ(npcs.size(), 1);
    
    // Должны быть сообщения об ошибках
    EXPECT_NE(output.find("Ошибка загрузки NPC"), std::string::npos);
}

// ==================== ТЕСТЫ СОХРАНЕНИЯ В ФАЙЛ ====================

TEST_F(FactoryTest, SaveToFile) {
    // Создаем тестовых NPC
    std::vector<std::shared_ptr<NPC>> npcs;
    npcs.push_back(std::make_shared<Bear>("Медведь_1", Point(8, 4), 5, 10));
    npcs.push_back(std::make_shared<Orc>("Орк_2", Point(0, 6), 3, 8));
    npcs.push_back(std::make_shared<Squirrel>("Белка_3", Point(5, 7), 4, 2));
    
    // Помечаем одного NPC мертвым
    npcs[1]->markDead();
    
    // Сохраняем в файл
    factory.saveToFile("../data/test_output.txt", npcs);
    
    // Проверяем содержимое файла
    std::ifstream file("../data/test_output.txt");
    std::string line;
    std::vector<std::string> lines;
    
    while (std::getline(file, line)) {
        lines.push_back(line);
    }
    
    // Должны сохраниться только живые NPC (2 из 3)
    EXPECT_EQ(lines.size(), 2);
    
    // Проверяем формат первой строки (Медведь)
    EXPECT_NE(lines[0].find("Медведь Медведь_1 8 4 5 10"), std::string::npos);
    
    // Проверяем формат второй строки (Белка)
    EXPECT_NE(lines[1].find("Белка Белка_3 5 7 4 2"), std::string::npos);
    
    // Мертвый орк не должен сохраниться
    bool orcFound = false;
    for (const auto& line : lines) {
        if (line.find("Орк_2") != std::string::npos) {
            orcFound = true;
            break;
        }
    }
    EXPECT_FALSE(orcFound);
}

TEST_F(FactoryTest, SaveToFileInvalidPath) {
    std::vector<std::shared_ptr<NPC>> npcs;
    npcs.push_back(std::make_shared<Bear>("Медведь_1", Point(1, 1), 5, 10));
    
    // Попытка сохранить в несуществующий каталог
    EXPECT_THROW(factory.saveToFile("/nonexistent/path/test.txt", npcs), std::runtime_error);
}

TEST_F(FactoryTest, SaveEmptyVector) {
    std::vector<std::shared_ptr<NPC>> npcs;
    
    // Сохраняем пустой вектор
    EXPECT_NO_THROW(factory.saveToFile("../data/test_output.txt", npcs));
    
    // Проверяем что файл создан и пуст
    std::ifstream file("../data/test_output.txt");
    EXPECT_TRUE(file.is_open());
    
    std::string line;
    int lineCount = 0;
    while (std::getline(file, line)) {
        lineCount++;
    }
    
    EXPECT_EQ(lineCount, 0);
}

// ==================== ТЕСТЫ КРУГООБОРОТА ====================

TEST_F(FactoryTest, RoundTrip) {
    // Создаем тестовых NPC
    std::vector<std::shared_ptr<NPC>> originalNpcs;
    originalNpcs.push_back(std::make_shared<Bear>("Медведь_1", Point(8, 4), 5, 10));
    originalNpcs.push_back(std::make_shared<Orc>("Орк_2", Point(0, 6), 3, 8));
    originalNpcs.push_back(std::make_shared<Squirrel>("Белка_3", Point(5, 7), 4, 2));
    
    // Сохраняем
    factory.saveToFile("../data/test_roundtrip.txt", originalNpcs);
    
    // Загружаем
    auto loadedNpcs = factory.loadFromFile("../data/test_roundtrip.txt");
    
    // Проверяем что загрузилось столько же NPC
    EXPECT_EQ(loadedNpcs.size(), originalNpcs.size());
    
    // Проверяем данные NPC (игнорируем ID, так как они генерируются заново)
    for (size_t i = 0; i < loadedNpcs.size(); ++i) {
        EXPECT_EQ(loadedNpcs[i]->getType(), originalNpcs[i]->getType());
        EXPECT_EQ(loadedNpcs[i]->getPos(), originalNpcs[i]->getPos());
        EXPECT_EQ(loadedNpcs[i]->getMoveRange(), originalNpcs[i]->getMoveRange());
        EXPECT_EQ(loadedNpcs[i]->getAttackRange(), originalNpcs[i]->getAttackRange());
    }
}

// ==================== ТЕСТЫ CREATOR КЛАССОВ ====================

TEST_F(FactoryTest, BearCreatorDirect) {
    BearCreator creator;
    
    // Тест создания с случайной позицией
    auto bear1 = creator.createNPC("Медведь_1");
    EXPECT_NE(bear1, nullptr);
    EXPECT_EQ(bear1->getType(), "Медведь");
    EXPECT_EQ(bear1->getID(), "Медведь_1");
    
    // Тест создания с заданной позицией
    Point testPos(5, 10);
    auto bear2 = creator.createNPC("Медведь_2", testPos);
    EXPECT_EQ(bear2->getPos(), testPos);
    
    // Тест создания с полными параметрами (moveRange и attackRange игнорируются в текущей реализации)
    auto bear3 = creator.createNPC("Медведь_3", testPos, 7, 12);
    EXPECT_EQ(bear3->getPos(), testPos);
    // В текущей реализации Bear использует параметры из GameConfig, а не переданные
    EXPECT_EQ(bear3->getMoveRange(), GameConfig::Bear::MOVE_RANGE);
    EXPECT_EQ(bear3->getAttackRange(), GameConfig::Bear::ATTACK_RANGE);
}

TEST_F(FactoryTest, OrcCreatorDirect) {
    OrcCreator creator;
    
    auto orc1 = creator.createNPC("Орк_1");
    EXPECT_NE(orc1, nullptr);
    EXPECT_EQ(orc1->getType(), "Орк");
    
    Point testPos(3, 4);
    auto orc2 = creator.createNPC("Орк_2", testPos);
    EXPECT_EQ(orc2->getPos(), testPos);
    
    auto orc3 = creator.createNPC("Орк_3", testPos, 6, 9);
    EXPECT_EQ(orc3->getPos(), testPos);
    EXPECT_EQ(orc3->getMoveRange(), GameConfig::Orc::MOVE_RANGE);
    EXPECT_EQ(orc3->getAttackRange(), GameConfig::Orc::ATTACK_RANGE);
}

TEST_F(FactoryTest, SquirrelCreatorDirect) {
    SquirrelCreator creator;
    
    auto squirrel1 = creator.createNPC("Белка_1");
    EXPECT_NE(squirrel1, nullptr);
    EXPECT_EQ(squirrel1->getType(), "Белка");
    
    Point testPos(7, 8);
    auto squirrel2 = creator.createNPC("Белка_2", testPos);
    EXPECT_EQ(squirrel2->getPos(), testPos);
    
    auto squirrel3 = creator.createNPC("Белка_3", testPos, 8, 3);
    EXPECT_EQ(squirrel3->getPos(), testPos);
    EXPECT_EQ(squirrel3->getMoveRange(), GameConfig::Squirrel::MOVE_RANGE);
    EXPECT_EQ(squirrel3->getAttackRange(), GameConfig::Squirrel::ATTACK_RANGE);
}

// ==================== ТЕСТЫ NPCCREATOR ====================

TEST_F(FactoryTest, NPCCreatorRandomPosition) {
    // Создаем наследника для теста базового класса
    class TestCreator : public NPCCreator {
    public:
        std::shared_ptr<NPC> createNPC(const std::string& id) override { return nullptr; }
        std::shared_ptr<NPC> createNPC(const std::string& id, const Point& p) override { return nullptr; }
        std::shared_ptr<NPC> createNPC(const std::string& id, const Point& p, int moveRange, int attackRange) override { return nullptr; }
        
        Point testGenerateRandomPosition() const {
            return generateRandomPosition();
        }
    };
    
    TestCreator creator;
    
    // Генерируем несколько позиций и проверяем границы
    for (int i = 0; i < 1000; ++i) {
        Point pos = creator.testGenerateRandomPosition();
        
        EXPECT_GE(pos.getX(), 0);
        EXPECT_LT(pos.getX(), GameConfig::MAP_WIDTH);
        EXPECT_GE(pos.getY(), 0);
        EXPECT_LT(pos.getY(), GameConfig::MAP_HEIGHT);
    }
}

// ==================== ТЕСТЫ ГРАНИЧНЫХ СЛУЧАЕВ ====================

TEST_F(FactoryTest, BoundaryPositions) {
    // Тест позиций на границах карты
    Point topLeft(0, 0);
    Point bottomRight(GameConfig::MAP_WIDTH - 1, GameConfig::MAP_HEIGHT - 1);
    Point middle(GameConfig::MAP_WIDTH / 2, GameConfig::MAP_HEIGHT / 2);
    
    auto orc1 = factory.createNPC("Орк", topLeft);
    auto bear1 = factory.createNPC("Медведь", bottomRight);
    auto squirrel1 = factory.createNPC("Белка", middle);
    
    EXPECT_EQ(orc1->getPos(), topLeft);
    EXPECT_EQ(bear1->getPos(), bottomRight);
    EXPECT_EQ(squirrel1->getPos(), middle);
}

TEST_F(FactoryTest, CreateManyNPCs) {
    // Создаем много NPC для проверки стабильности
    std::vector<std::shared_ptr<NPC>> npcs;
    
    for (int i = 0; i < 100; ++i) {
        auto npc = factory.createRandomNPC();
        EXPECT_NE(npc, nullptr);
        EXPECT_TRUE(npc->isAlive());
        npcs.push_back(npc);
    }
    
    // Все NPC должны быть уникальными
    std::set<std::string> ids;
    for (const auto& npc : npcs) {
        ids.insert(npc->getID());
    }
    
    EXPECT_EQ(ids.size(), npcs.size());
}

// ==================== ТЕСТЫ С ТИПАМИ В РАЗНОМ РЕГИСТРЕ ====================

TEST_F(FactoryTest, CaseSensitiveTypeNames) {
    // Типы чувствительны к регистру (если так задумано)
    EXPECT_THROW(factory.createNPC("орк"), std::invalid_argument);
    EXPECT_THROW(factory.createNPC("МЕДВЕДЬ"), std::invalid_argument);
    EXPECT_THROW(factory.createNPC("белка"), std::invalid_argument);
    
    // Правильные варианты должны работать
    EXPECT_NO_THROW(factory.createNPC("Орк"));
    EXPECT_NO_THROW(factory.createNPC("Медведь"));
    EXPECT_NO_THROW(factory.createNPC("Белка"));
}