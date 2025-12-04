#pragma once

#include "./Observer.h"
#include <vector>
#include <algorithm>

class ISubject {
protected:
    std::vector<std::shared_ptr<IObserver>> observers;

public:
    virtual ~ISubject() = default;

    void attach(std::shared_ptr<IObserver> observer) {
        observers.emplace_back(observer);
    }

    void detach(std::shared_ptr<IObserver> observer) {
        observers.erase(
            std::remove(observers.begin(), observers.end(), observer),
            observers.end()
        );
    }

    void notify(const std::string& event) {
        for (auto& observer : observers) {
            observer->update(event);
        }
    }
};