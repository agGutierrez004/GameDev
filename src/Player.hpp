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

    struct Upgrade {
        std::string name;
        int cost = 0;                          // coins needed for purchase
        std::function<void(Player&)> apply;
    };

    void OfferUpgrades();

    Room* room = nullptr;
    int m_keyCount = 0;        // keys collected
    int coins = 0;            // distinct coin currency
    int health = 10;
    int maxHealth = 10;        // current maximum health
    int m_attackDice = 1;      // number of extra dice for player attacks
    int coinBonus = 0;         // extra coins on pickup

private:
    static const std::vector<Upgrade>& GetAllUpgrades();
};
