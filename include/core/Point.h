#pragma once
#include <iostream>
#include <cmath>
#include <stdexcept>
#include <algorithm>
#include "./Config.h"

class Point {
private:
    int x, y;

public:
    Point(): x(0), y(0) {}
    Point(int x, int y) : x(x), y(y) {}

    int getX() const {return x;}
    int getY() const {return y;}

    void setX(int newX) {
        newX = std::max(0, std::min(newX, GameConfig::MAP_WIDTH - 1));
        if (newX != x) {
            x = newX;
        }
    }

    void setY(int newY) {
        newY = std::max(0, std::min(newY, GameConfig::MAP_HEIGHT - 1));
        if (newY != y) {
            y = newY;
        }
    }

    void setCoordinates(int newX, int newY) {
        newX = std::max(0, std::min(newX, GameConfig::MAP_WIDTH - 1));
        newY = std::max(0, std::min(newY, GameConfig::MAP_HEIGHT - 1));
        
        if (newX != x) {
            x = newX;
        }
        if (newY != y) {
            y = newY;
        }
    }

    double distanceTo(const Point& other) const {
        int dx = x - other.x;
        int dy = y - other.y;
        return sqrt(dx * dx + dy * dy);
    }

    Point move(int dx, int dy) const {
        int newX = x + dx;
        int newY = y + dy;
        newX = std::clamp(newX, 0, GameConfig::MAP_WIDTH - 1);
        newY = std::clamp(newY, 0, GameConfig::MAP_HEIGHT - 1);

        return Point(newX, newY);
    }

    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }

    bool operator!=(const Point& other) const {
        return !(*this==other);
    }

    friend std::ostream &operator<<(std::ostream& os, const Point &p) {
        os << p.x << " " << p.y;
        return os;
    }

    friend std::istream &operator>>(std::istream& is, Point &p) {
        is >> p.x >> p.y;
        return is;
    }
};