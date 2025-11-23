#pragma once

#include "./subject.h"
#include <iostream>

class ConsoleLogger : public IObserver {
public:
    void update(const std::string& killEvent) override {
        std::cout << "[Console log] " << killEvent << std::endl;
    }
};