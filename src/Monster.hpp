#pragma once

#include "Entity.hpp"

class Room;

class Monster : public Entity
{
public:
    void Start(Vec2 _pos);
    void Update();
    Room* room = nullptr;
    int health = 5;
};