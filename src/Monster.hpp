#pragma once
#include <string>
#include <iostream>
using namespace std;

#include "Entity.hpp"

class Room;

enum class MonsterType {
    Goblin,
    Orc
};

class Monster : public Entity
{
public:
    void Start(Vec2 _pos);
    void Update();
    Room* room = nullptr;

    string name;
    int health;
    int attack;
    MonsterType type;

    Monster(string n, int h, int a) : name(n), health(h), attack(a), type(MonsterType::Goblin) {}
    
    Monster(MonsterType t) : type(t) {
        switch (t) {
        case MonsterType::Goblin:
            name       = "Goblin";
            m_character = 'g';           // lower‑case g on the map
            health     = 5;
            attack     = 1;
            break;
        case MonsterType::Orc:
            name       = "Orc";
            m_character = 'o';           // o for orc
            health     = 10;
            attack     = 2;
            break;
        }
    }

    // Method to display all stats
    void display() {
        cout << name << " - HP: " << health << ", ATK: " << attack << endl;
    }
};