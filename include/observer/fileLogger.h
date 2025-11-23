#pragma once

#include "./observer.h"
#include <fstream>
#include <stdexcept>


class FileLogger : public IObserver {
private:
    std::string filename;

public:
    FileLogger(const std::string& filename) : filename(filename) {}

    void update(const std::string& killEvent) override {
        std::ofstream file(filename, std::ios::app);    // std::ios::app - открытие файла в режиме "записи в конец" 

        if (file.is_open()) {
            file << "[File log] " << killEvent << std::endl;
        }
    }
};