#pragma once

#include "Entity.hpp"
#include <functional>
#include <string>
#include <vector>

class Player : public Entity
{
public:
    void Start(Vec2 _pos);
    void Update();
    void PrintStats();
    void Death();
    void Heal(int amount);
    void OfferUpgrades();

    struct Upgrade {
        std::string name;
        int cost = 0;
        std::function<void(Player&)> apply;
    };

    Room* room = nullptr;
    int m_keyCount = 0;
    int coins = 0;
    int health = 10;
    int maxHealth = 10;
    int m_attackDice = 1;
    int coinBonus = 0;

private:
    static const std::vector<Upgrade>& GetAllUpgrades();
};