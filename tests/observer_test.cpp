#include <gtest/gtest.h>
#include <fstream>
#include <sstream>
#include <filesystem>
#include "../include/observer/battleLogger.h"
#include "../include/observer/consoleLogger.h"
#include "../include/observer/fileLogger.h"
#include "../include/npc/elf.h"
#include "../include/npc/dragon.h"
#include "../include/npc/druid.h"

class ObserverTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Очищаем тестовые файлы перед каждым тестом
        if (std::filesystem::exists("../data/test_log.txt")) {
            std::filesystem::remove("../data/test_log.txt");
        }
        if (std::filesystem::exists("../data/test_empty_log.txt")) {
            std::filesystem::remove("../data/test_empty_log.txt");
        }
    }
    
    void TearDown() override {
        // Убираем тестовые файлы после тестов
        if (std::filesystem::exists("../data/test_log.txt")) {
            std::filesystem::remove("../data/test_log.txt");
        }
        if (std::filesystem::exists("../data/test_empty_log.txt")) {
            std::filesystem::remove("../data/test_empty_log.txt");
        }
    }
};

// Тесты базового ISubject
TEST_F(ObserverTest, SubjectAttachDetach) {
    auto subject = std::make_shared<BattleLogger>();
    auto observer1 = std::make_shared<ConsoleLogger>();
    auto observer2 = std::make_shared<ConsoleLogger>();
    
    // Прикрепляем наблюдателей
    subject->attach(observer1);
    subject->attach(observer2);
    
    // Должны быть 2 наблюдателя
    // (проверяем через нотификацию)
    testing::internal::CaptureStdout();
    subject->logBattleStart(2);
    std::string output = testing::internal::GetCapturedStdout();
    
    // Должно быть 2 сообщения (по одному на каждого наблюдателя)
    size_t count = 0;
    size_t pos = 0;
    while ((pos = output.find("[Console log]", pos)) != std::string::npos) {
        count++;
        pos += std::string("[Console log]").length();
    }
    EXPECT_EQ(count, 2);
    
    // Открепляем одного наблюдателя
    subject->detach(observer1);
    
    testing::internal::CaptureStdout();
    subject->logBattleEnd(1);
    output = testing::internal::GetCapturedStdout();
    
    // Должно остаться 1 сообщение
    count = 0;
    pos = 0;
    while ((pos = output.find("[Console log]", pos)) != std::string::npos) {
        count++;
        pos += std::string("[Console log]").length();
    }
    EXPECT_EQ(count, 1);
}

TEST_F(ObserverTest, SubjectNotifyMultipleObservers) {
    auto subject = std::make_shared<BattleLogger>();
    auto consoleObserver = std::make_shared<ConsoleLogger>();
    auto fileObserver = std::make_shared<FileLogger>("../data/test_log.txt");
    
    subject->attach(consoleObserver);
    subject->attach(fileObserver);
    
    // Нотифицируем всех наблюдателей
    testing::internal::CaptureStdout();
    subject->notify("Тестовое событие");
    std::string consoleOutput = testing::internal::GetCapturedStdout();
    
    // Проверяем вывод в консоль
    EXPECT_NE(consoleOutput.find("[Console log] Тестовое событие"), std::string::npos);
    
    // Проверяем запись в файл
    std::ifstream file("../data/test_log.txt");
    std::string fileContent;
    std::getline(file, fileContent);
    EXPECT_NE(fileContent.find("[File log] Тестовое событие"), std::string::npos);
}

TEST_F(ObserverTest, SubjectNoObservers) {
    auto subject = std::make_shared<BattleLogger>();
    
    // Нотификация без наблюдателей не должна падать
    EXPECT_NO_THROW(subject->notify("Событие без наблюдателей"));
    
    // Проверяем что нет вывода в консоль
    testing::internal::CaptureStdout();
    subject->notify("Тест");
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(output.empty());
}

// Тесты ConsoleLogger
TEST_F(ObserverTest, ConsoleLoggerOutput) {
    ConsoleLogger logger;
    
    testing::internal::CaptureStdout();
    logger.update("Эльф победил Дракона");
    std::string output = testing::internal::GetCapturedStdout();
    
    EXPECT_EQ(output, "[Console log] Эльф победил Дракона\n");
}

TEST_F(ObserverTest, ConsoleLoggerMultipleUpdates) {
    ConsoleLogger logger;
    
    testing::internal::CaptureStdout();
    logger.update("Событие 1");
    logger.update("Событие 2");
    logger.update("Событие 3");
    std::string output = testing::internal::GetCapturedStdout();
    
    EXPECT_NE(output.find("[Console log] Событие 1"), std::string::npos);
    EXPECT_NE(output.find("[Console log] Событие 2"), std::string::npos);
    EXPECT_NE(output.find("[Console log] Событие 3"), std::string::npos);
}

// Тесты FileLogger
TEST_F(ObserverTest, FileLoggerCreatesFile) {
    FileLogger logger("../data/test_log.txt");
    
    // Файл не должен существовать до обновления
    EXPECT_FALSE(std::filesystem::exists("../data/test_log.txt"));
    
    logger.update("Первое событие");
    
    // Файл должен быть создан
    EXPECT_TRUE(std::filesystem::exists("../data/test_log.txt"));
}

TEST_F(ObserverTest, FileLoggerAppendsMessages) {
    FileLogger logger("../data/test_log.txt");
    
    logger.update("Сообщение 1");
    logger.update("Сообщение 2");
    logger.update("Сообщение 3");
    
    // Проверяем содержимое файла
    std::ifstream file("../data/test_log.txt");
    std::string line;
    std::vector<std::string> lines;
    
    while (std::getline(file, line)) {
        lines.push_back(line);
    }
    
    EXPECT_EQ(lines.size(), 3);
    EXPECT_EQ(lines[0], "[File log] Сообщение 1");
    EXPECT_EQ(lines[1], "[File log] Сообщение 2");
    EXPECT_EQ(lines[2], "[File log] Сообщение 3");
}

TEST_F(ObserverTest, FileLoggerInvalidPath) {
    // Попытка создать файл в несуществующей директории
    FileLogger logger("/invalid/path/test_log.txt");
    
    // Не должно падать, просто не создаст файл
    EXPECT_NO_THROW(logger.update("Тестовое сообщение"));
}

// Тесты BattleLogger
TEST_F(ObserverTest, BattleLoggerLogKill) {
    auto battleLogger = std::make_shared<BattleLogger>();
    auto consoleObserver = std::make_shared<ConsoleLogger>();
    battleLogger->attach(consoleObserver);
    
    Point pos(100, 100);
    Elf elf("Леголас", pos, 10);
    Dragon dragon("Смауг", pos, 10);
    
    testing::internal::CaptureStdout();
    battleLogger->logKill(&elf, &dragon);
    std::string output = testing::internal::GetCapturedStdout();
    
    EXPECT_NE(output.find("Леголас (Эльф) убил Смауг (Дракон)"), std::string::npos);
}

TEST_F(ObserverTest, BattleLoggerLogBattleStart) {
    auto battleLogger = std::make_shared<BattleLogger>();
    auto consoleObserver = std::make_shared<ConsoleLogger>();
    battleLogger->attach(consoleObserver);
    
    testing::internal::CaptureStdout();
    battleLogger->logBattleStart(5);
    std::string output = testing::internal::GetCapturedStdout();
    
    EXPECT_NE(output.find("=== НАЧАЛО БОЯ ==="), std::string::npos);
}

TEST_F(ObserverTest, BattleLoggerLogBattleEnd) {
    auto battleLogger = std::make_shared<BattleLogger>();
    auto consoleObserver = std::make_shared<ConsoleLogger>();
    battleLogger->attach(consoleObserver);
    
    testing::internal::CaptureStdout();
    battleLogger->logBattleEnd(2);
    std::string output = testing::internal::GetCapturedStdout();
    
    EXPECT_NE(output.find("=== КОНЕЦ БОЯ. Выжило: 2 ==="), std::string::npos);
}

TEST_F(ObserverTest, BattleLoggerMultipleEvents) {
    auto battleLogger = std::make_shared<BattleLogger>();
    auto fileObserver = std::make_shared<FileLogger>("../data/test_log.txt");
    battleLogger->attach(fileObserver);
    
    Point pos(100, 100);
    Elf elf("Эльф", pos, 10);
    Dragon dragon("Дракон", pos, 10);
    Druid druid("Друид", pos, 10);
    
    // Логируем несколько событий
    battleLogger->logBattleStart(3);
    battleLogger->logKill(&elf, &druid);
    battleLogger->logKill(&dragon, &elf);
    battleLogger->logBattleEnd(1);
    
    // Проверяем файл
    std::ifstream file("../data/test_log.txt");
    std::string line;
    std::vector<std::string> lines;
    
    while (std::getline(file, line)) {
        lines.push_back(line);
    }
    
    EXPECT_EQ(lines.size(), 4);
    EXPECT_NE(lines[0].find("=== НАЧАЛО БОЯ ==="), std::string::npos);
    EXPECT_NE(lines[1].find("Эльф (Эльф) убил Друид (Друид)"), std::string::npos);
    EXPECT_NE(lines[2].find("Дракон (Дракон) убил Эльф (Эльф)"), std::string::npos);
    EXPECT_NE(lines[3].find("=== КОНЕЦ БОЯ. Выжило: 1 ==="), std::string::npos);
}

// Тесты интеграции всех компонентов
TEST_F(ObserverTest, FullIntegration) {
    // Создаем полную систему
    auto battleLogger = std::make_shared<BattleLogger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>();
    auto fileLogger = std::make_shared<FileLogger>("../data/test_log.txt");
    
    battleLogger->attach(consoleLogger);
    battleLogger->attach(fileLogger);
    
    Point pos1(100, 100), pos2(105, 105);
    Elf elf("Леголас", pos1, 10);
    Druid druid("Мерлин", pos2, 10);
    
    // Логируем бой
    testing::internal::CaptureStdout();
    battleLogger->logBattleStart(2);
    battleLogger->logKill(&elf, &druid);
    battleLogger->logBattleEnd(1);
    std::string consoleOutput = testing::internal::GetCapturedStdout();
    
    // Проверяем консольный вывод
    EXPECT_NE(consoleOutput.find("=== НАЧАЛО БОЯ ==="), std::string::npos);
    EXPECT_NE(consoleOutput.find("Леголас (Эльф) убил Мерлин (Друид)"), std::string::npos);
    EXPECT_NE(consoleOutput.find("=== КОНЕЦ БОЯ. Выжило: 1 ==="), std::string::npos);
    
    // Проверяем файловый вывод
    std::ifstream file("../data/test_log.txt");
    std::string fileContent;
    std::string line;
    while (std::getline(file, line)) {
        fileContent += line + "\n";
    }
    
    EXPECT_NE(fileContent.find("=== НАЧАЛО БОЯ ==="), std::string::npos);
    EXPECT_NE(fileContent.find("Леголас (Эльф) убил Мерлин (Друид)"), std::string::npos);
    EXPECT_NE(fileContent.find("=== КОНЕЦ БОЯ. Выжило: 1 ==="), std::string::npos);
}

// Тесты с разными типами NPC
TEST_F(ObserverTest, DifferentNPCTypesLogging) {
    auto battleLogger = std::make_shared<BattleLogger>();
    auto fileLogger = std::make_shared<FileLogger>("../data/test_log.txt");
    battleLogger->attach(fileLogger);
    
    Point pos(100, 100);
    
    // Тестируем все комбинации NPC
    Elf elf("Эльф", pos, 10);
    Dragon dragon("Дракон", pos, 10);
    Druid druid("Друид", pos, 10);
    
    battleLogger->logKill(&elf, &druid);
    battleLogger->logKill(&dragon, &elf);
    battleLogger->logKill(&druid, &dragon);
    
    // Проверяем файл
    std::ifstream file("../data/test_log.txt");
    std::string content;
    std::string line;
    while (std::getline(file, line)) {
        content += line + "\n";
    }
    
    EXPECT_NE(content.find("Эльф (Эльф) убил Друид (Друид)"), std::string::npos);
    EXPECT_NE(content.find("Дракон (Дракон) убил Эльф (Эльф)"), std::string::npos);
    EXPECT_NE(content.find("Друид (Друид) убил Дракон (Дракон)"), std::string::npos);
}

// Тесты детачей наблюдателей
TEST_F(ObserverTest, ObserverDetachDuringOperation) {
    auto battleLogger = std::make_shared<BattleLogger>();
    auto observer1 = std::make_shared<ConsoleLogger>();
    auto observer2 = std::make_shared<ConsoleLogger>();
    
    battleLogger->attach(observer1);
    battleLogger->attach(observer2);
    
    // Детach во время операции не должен вызывать проблем
    testing::internal::CaptureStdout();
    battleLogger->notify("Сообщение 1");
    battleLogger->detach(observer1);
    battleLogger->notify("Сообщение 2");
    battleLogger->detach(observer2);
    battleLogger->notify("Сообщение 3"); // Никто не слушает
    std::string output = testing::internal::GetCapturedStdout();
    
    // Должны быть 3 сообщения (2 наблюдателя для первого, 1 для второго)
    size_t count = 0;
    size_t pos = 0;
    while ((pos = output.find("[Console log]", pos)) != std::string::npos) {
        count++;
        pos += std::string("[Console log]").length();
    }
    EXPECT_EQ(count, 3);
}

// Тесты пустых и специальных сообщений
TEST_F(ObserverTest, EmptyAndSpecialMessages) {
    auto battleLogger = std::make_shared<BattleLogger>();
    auto consoleObserver = std::make_shared<ConsoleLogger>();
    battleLogger->attach(consoleObserver);
    
    testing::internal::CaptureStdout();
    battleLogger->notify(""); // Пустое сообщение
    battleLogger->notify("Сообщение с спецсимволами !@#$%^&*()");
    battleLogger->notify("Очень длинное сообщение " + std::string(100, 'x'));
    std::string output = testing::internal::GetCapturedStdout();
    
    EXPECT_NE(output.find("[Console log] "), std::string::npos);
    EXPECT_NE(output.find("Сообщение с спецсимволами"), std::string::npos);
}