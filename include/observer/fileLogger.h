#pragma once

#include "./Subject.h"
#include <fstream>

class FileLogger : public IObserver {
private:
    std::string filename;

public:
    FileLogger(const std::string& filename) : filename(filename) {}

    void update(const std::string& event) override {
        std::ofstream file(filename, std::ios::app);
        if (file.is_open()) {
            file << "[БОЙ] " << event << std::endl;
        }
    }
};