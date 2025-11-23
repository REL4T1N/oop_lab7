#include "../../include/observer/subject.h"
#include <algorithm>

// Реализация методов ISubject
void ISubject::attach(std::shared_ptr<IObserver> observer) {
    observers.push_back(observer);
}

void ISubject::detach(std::shared_ptr<IObserver> observer) {
    observers.erase(
        std::remove(observers.begin(), observers.end(), observer),
        observers.end()
    );
}

void ISubject::notify(const std::string& killEvent) {
    for (auto& observer : observers) {
        observer->update(killEvent);
    }
}