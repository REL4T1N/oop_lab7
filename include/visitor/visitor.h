#pragma once

// объявление заранее чтобы не было цикличесих импортов
class Elf;
class Dragon;
class Druid;


class Visitor {
public:
    virtual ~Visitor() =  default;

    virtual void visit(Elf& elf) = 0;
    virtual void visit(Dragon& dragon) = 0;
    virtual void visit(Druid& druid) = 0;
};