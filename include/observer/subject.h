#pragma once

#include "./observer.h"
#include <vector>
#include <memory>

class ISubject {
protected:
    std::vector<std::shared_ptr<IObserver>> observers;

public:
    virtual ~ISubject() = default;
    virtual void attach(std::shared_ptr<IObserver> observer);
    virtual void detach(std::shared_ptr<IObserver> observer);
    virtual void notify(const std::string& killEvent); 
};