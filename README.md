# Боже упаси
---
**Студент**: Волков Алексей Александрович  
**Учебная группа**: M8О-201БВ-24  
**Контакты**: volckovl3sch@yandex.ru  
**Работа выполнена**: 05.12.25  
**Преподаватель**: Дзюба Дмитрий Владимирович  

---

## Структура проекта

```txt
oop_lab7/
├── include/
│   ├── behavior/                     # Поведение и корутины
│   │   ├── CoroutineManager.h
│   │   ├── Dice.h
│   │   └── NPCBehavior.h
│   ├── core/                        # Ядро игры
│   │   ├── Config.h                 # Конфигурация игры
│   │   ├── Game.h                   # Основной класс игры
│   │   ├── GameTimer.h              # Таймер игры
│   │   ├── GameWorld.h              # Игровой мир
│   │   ├── Point.h                  # Класс Point для координат
│   │   └── ThreadSafeConsole.h      # Потокобезопасная консоль
│   ├── factory/
│   │   ├── NPCCreator.h             # Интерфейс создателя NPC
│   │   ├── NPCFactory.h             # Фабрика NPC
│   │   ├── BearCreator.h            # Создатель медведей
│   │   ├── OrcCreator.h             # Создатель орков
│   │   └── SquirrelCreator.h        # Создатель белок
│   ├── npc/
│   │   ├── NPC.h                    # Базовый интерфейс NPC
│   │   ├── Bear.h                   # Класс Bear
│   │   ├── Orc.h                    # Класс Orc
│   │   └── Squirrel.h               # Класс Squirrel
│   ├── observer/
│   │   ├── Observer.h               # Интерфейс Observer
│   │   ├── Subject.h                # Интерфейс Subject
│   │   ├── BattleLogger.h           # Логгер боевых событий
│   │   ├── ConsoleLogger.h          # Вывод в консоль
│   │   └── FileLogger.h             # Запись в файл
│   ├── render/                      # Рендеринг (ваше расширение)
│   │   └── MapRenderer.h            # Рендерер карты
│   └── visitor/
│       ├── Visitor.h                # Интерфейс Visitor
│       └── AttackVisitor.h          # Посетитель для атак
├── src/
│   ├── behavior/
│   │   └── CoroutineManager.cpp
│   ├── core/
│   │   ├── Game.cpp
│   │   ├── GameTimer.cpp
│   │   └── GameWorld.cpp
│   ├── factory/
│   │   ├── NPCFactory.cpp
│   │   ├── NPCCreator.cpp
│   │   ├── BearCreator.cpp
│   │   ├── OrcCreator.cpp
│   │   └── SquirrelCreator.cpp
│   ├── npc/
│   │   ├── NPC.cpp
│   │   ├── Bear.cpp
│   │   ├── Orc.cpp
│   │   └── Squirrel.cpp
│   ├── observer/
│   │   └── BattleLogger.cpp
│   ├── render/
│   │   └── MapRenderer.cpp
│   └── visitor/
│       └── AttackVisitor.cpp
├── tests/
│   ├── behaviorAndCore_test.cpp     # Тесты поведения и яда
│   ├── factory_test.cpp             # Тесты фабрики
│   ├── npc_test.cpp                 # Тесты NPC
│   ├── observerAndVisitor_test.cpp  # Тесты наблюдателей и посетителей
│   ├── render_test.cpp              # Тесты рендеринга
│   └── main_test.cpp                # Главный файл тестов
├── data/                            # Файлы данных
├── main.cpp                         # Демонстрационная программа
├── CMakeLists.txt                   # Система сборки
└── README.md                        # Документация
```

---

## Сборка и запуск

### Требования
- Компилятор C++ с поддержкой стандарта C++17
- CMake версии 3.14 или выше
- Google Test для тестирования

### Инструкция по сборке:
```bash
# Сборка
mkdir build
cd build
cmake ..
make

# Запуск демонстрационной программы
./dungeon_editor

# Запуск тестов
./tests

