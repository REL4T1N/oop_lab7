#pragma once

#include "../npc/NPC.h"
#include <vector>
#include <thread>
#include <atomic>
#include <mutex>
#include <coroutine>
#include <chrono>

class GameWorld;
class Dice;
class BattleLogger;

// генератор поведения корутин
template<typename T>
struct BehaviorGenerator {
    struct promise_type {
        T cur_value;
        bool finished = false;

        BehaviorGenerator get_return_object() {
            return BehaviorGenerator{std::coroutine_handle<promise_type>::from_promise(*this)};
        }

        std::suspend_always initial_suspend() {return {};}
        std::suspend_always final_suspend() noexcept {return {};}

        std::suspend_always yield_value(T value) {
            cur_value = value;
            return {};
        }

        void return_void() {finished =true;}
        void unhandled_exception() {std::terminate();}
    };

    std::coroutine_handle<promise_type> coro;

    explicit BehaviorGenerator(std::coroutine_handle<promise_type> h) : coro(h) {}
    ~BehaviorGenerator() {if (coro) coro.destroy();}

    BehaviorGenerator(BehaviorGenerator&& other) noexcept : coro(other.coro) {other.coro = nullptr;}
    BehaviorGenerator(const BehaviorGenerator&) = delete;
    BehaviorGenerator& operator=(const BehaviorGenerator&) = delete;

    bool next() {
        if (!coro.done()) {
            coro.resume();
            return !coro.done();
        }
        return false;
    }

    T value() const {return coro.promise().cur_value;}
};

// менеджер корутин npc
class CoroutineManager {
private:
    struct NPCCoroutine {
        std::shared_ptr<NPC> npc;
        BehaviorGenerator<bool> behavior;

        NPCCoroutine(std::shared_ptr<NPC> n, BehaviorGenerator<bool> b) : npc(n), behavior(std::move(b)) {}
    };

    std::shared_ptr<GameWorld> world;
    std::shared_ptr<Dice> dice;
    std::shared_ptr<BattleLogger> logger;

    std::vector<NPCCoroutine> coroutines;
    std::atomic<bool> running{false};
    std::thread coroutine_thread;
    mutable std::mutex mutex;

    void run();
    BehaviorGenerator<bool> createNPCBehavior(std::shared_ptr<NPC> npc);

public:
    CoroutineManager(std::shared_ptr<GameWorld> world, std::shared_ptr<Dice> dice, std::shared_ptr<BattleLogger> logger = nullptr);
    ~CoroutineManager();

    void addNPC(std::shared_ptr<NPC> npc);
    void start();
    void stop();
    std::vector<std::shared_ptr<NPC>> getAliveNPCs() const;
};