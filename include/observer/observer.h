#pragma once

#include <string>
#include <memory>

class IObserver {
public:
    virtual ~IObserver() = default;
    virtual void update(const std::string& event) = 0;
};