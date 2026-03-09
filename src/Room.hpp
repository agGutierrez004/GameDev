#pragma once
using namespace std; 
#include <vector>
#include "Door.hpp"
using namespace std;
class Entity;
class Player;
class Monster;

class Room {
public:
    void Load(string _path);
    void Draw();
    void Update();
    char GetLocation(Vec2 _pos);
    void ClearLocation(Vec2 _pos);
    void OpenDoor(Vec2 _pos);
    Monster* GetMonsterAt(Vec2 _pos);
    void RemoveMonster(Monster* _monster);
    bool HasMonsters() const { return !m_monsters.empty(); }
private:
    vector<Entity*> m_entities;
    Player* m_player = nullptr;
    vector<Monster*> m_monsters;
    vector<vector<char>> m_map;
    vector<Door> m_doors;
};