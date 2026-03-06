#pragma once

#include "Entity.hpp"

class Player : public Entity
{
public:
    void Start(Vec2 _pos);
    void Update();
    void PrintStats();
    void Death();
    void Heal(int amount);
    Room* room = nullptr;
    int m_keyCount = 0;        // keys collected
    int coins = 0;            // distinct coin currency
    int health = 10;
private:
};