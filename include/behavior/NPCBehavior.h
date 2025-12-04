#pragma once

#include "./CoroutineManager.h"
#include "./Dice.h"
#include "../visitor/AttackVisitor.h"
#include "../core/GameWorld.h"



class GameWorld; // предаврительное объявление

inline BehaviorGenerator<bool> createNPCBehavior(std::shared_ptr<NPC> npc, 
                                                std::shared_ptr<GameWorld> world, 
                                                std::shared_ptr<Dice> dice, 
                                                std::shared_ptr<BattleLogger> logger = nullptr) {

    while (npc->isAlive()) {
        // Перемещение
        try {
            Point newPos = npc->calculateNextPosition();
            npc->setPos(newPos);
            // if (logger) {
            //     logger->logGameEvent(
            //         npc->getID() + " перемещён на (" + std::to_string(newPos.getX()) + ", " + std::to_string(newPos.getY()) + ")"
            //     );
            // }
        }
        catch (const std::out_of_range&) {
            // Игнорируем
        }
        
        // Проверка боя
        auto nearbyNPCs = world->getNPCsInRange(npc, npc->getAttackRange());
        
        // Фильтруем только тех, кого можно атаковать
        std::vector<std::shared_ptr<NPC>> possibleTargets;
        for (auto& target : nearbyNPCs) {
            if (target->isAlive() && npc->canAttack(target)) {
                possibleTargets.push_back(target);
            }
        }

        if (!possibleTargets.empty()) {
            size_t index = dice->randomIndex(possibleTargets.size() - 1);
            auto& target = possibleTargets[index];
            
            int attackRoll = dice->rollAttack();
            int defenseRoll = dice->rollDefense();

            AttackVisitor visitor(target, logger);
            npc->accept(visitor);

            if (dice->resolveCombat(attackRoll, defenseRoll)) {
                target->markDead();
                if (logger) {
                    logger->logCombat(npc, target, true);
                }
            } else {
                if (logger) {
                    logger->logCombat(npc, target, false);
                }
            }
        }
        
        // ОДИН yield за цикл и небольшая задержка
        co_yield true;
        
        // Небольшая задержка чтобы не нагружать CPU
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    if (logger) {
        logger->logGameEvent(npc->getID() + " умер");
    }
}
