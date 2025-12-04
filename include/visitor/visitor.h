#pragma once

class Orc;
class Squirrel;
class Bear;

class Visitor {
public:
    virtual ~Visitor() = default;

    virtual void visit(Orc& orc) = 0;
    virtual void visit(Squirrel& squirrel) = 0;
    virtual void visit(Bear& bear) = 0;
};