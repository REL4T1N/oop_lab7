cmake_minimum_required(VERSION 3.14)
project(oop_lab6)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Включаем все заголовочные файлы
include_directories(include)

# Google Test
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.15.2
  TLS_VERIFY false
)
FetchContent_MakeAvailable(googletest)

# Основная программа
file(GLOB_RECURSE CPP_SOURCES "src/*.cpp")

add_executable(dungeon_editor
    main.cpp
    ${CPP_SOURCES}
)

# Создаем папку data в КОРНЕ проекта
add_custom_target(create_data_dir ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_SOURCE_DIR}/data
    COMMENT "Creating data directory in project root"
)

# ТЕСТЫ
add_executable(tests 
    tests/main_test.cpp
    tests/geom_test.cpp
    tests/npc_test.cpp
    tests/factory_test.cpp
    tests/visitor_test.cpp
    tests/observer_test.cpp
    ${CPP_SOURCES}  # Все исходники основного проекта
)

target_include_directories(tests PRIVATE include)
target_link_libraries(tests gtest gtest_main)

enable_testing()
add_test(NAME oop_lab6_tests COMMAND tests)