#pragma once

#include <random>
#include <chrono>

class Dice {
private:
    std::mt19937 generator; // что за mt19937
    std::uniform_int_distribution<int> distribution; // а это вообще что за шняга и зачем она нужна

public:
    Dice(int sides = 6) : generator(std::chrono::steady_clock::now().time_since_epoch().count()), distribution(1, sides) {}
    
    int rollAttack() {return distribution(generator);}
    int rollDefense() {return distribution(generator);}

    bool resolveCombat(int attackRoll, int defenseRoll) {
        return attackRoll > defenseRoll;
    }

    size_t randomIndex(size_t max) {
        std::uniform_int_distribution<size_t> dist(0, max);
        return dist(generator);  // Используем generator, а не rng!
    }
};