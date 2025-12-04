#pragma once

#include <iostream>
#include <mutex>
#include <sstream>

class ThreadSafeConsole {
private:
    static inline std::mutex cout_mutex;

public:
    template<typename T>
    static void print(const T& message) {
        std::lock_guard<std::mutex> lock(cout_mutex);
        std::cout << message;
    }

    template<typename T>
    static void println(const T& message) {
        std::lock_guard<std::mutex> lock(cout_mutex);
        std::cout << message << std::endl;
    }

    static void printStream(std::ostringstream& stream) {
        std::lock_guard<std::mutex> lock(cout_mutex);
        std::cout << stream.str();
        stream.str("");
    }
};

#define TS_PRINT(msg) ThreadSafeConsole::print(msg)
#define TS_PRINTLN(msg) ThreadSafeConsole::println(msg)