#pragma once

#include "./Subject.h"
#include "../core/ThreadSafeConsole.h"

class ConsoleLogger : public IObserver {
public:
    void update(const std::string& event) override {
        TS_PRINTLN("[БОЙ] " + event);
    }
};