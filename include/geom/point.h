#pragma once

#include <iostream>
#include <cmath>
#include <stdexcept>

class Point {
private:
    int x, y;

    void validate(int x, int y) const {
        if (x < 0 || x > 500 || y < 0 || y > 500) {
            throw std::out_of_range("Выход за пределы плоскости координат!");
        } 
    }

public:
    Point() : x(0), y(0) {}

    Point(int x, int y) : x(x), y(y) {
        validate(x, y);
    }

    int getX() const { return x; }
    int getY() const { return y; }

    void setX(int newX) {
        validate(newX, y);  
        if (newX != x) {
            x = newX;      
        }
    }

    void setY(int newY) {
        validate(x, newY);  
        if (newY != y) {
            y = newY;      
        }
    }

    void setCoordinates(int newX, int newY) {
        validate(newX, newY);  
        x = newX;
        y = newY;
    }

    double distanceTo(const Point& other) const {
        int dx = x - other.x;
        int dy = y - other.y;
        return sqrt(dx * dx + dy * dy);
    }

    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }

    bool operator!=(const Point& other) const {
        return !(*this == other);
    }

    friend std::ostream &operator<<(std::ostream &os, const Point &p) {
        os << p.x << " " << p.y;
        return os;
    }
};