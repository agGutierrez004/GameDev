#pragma once

#include <vector>
#include <string>
#include "Entity.hpp"
#include <functional>
#include "Door.hpp"
#include "fogpi/Math.hpp"

using namespace std; 

class Entity; 
class Player; 
class Monster; 

class Room{
    public: 
        void Load(string _path);
        void Draw(); 
        void Update(); 
        char GetLocation(Vec2 _pos); 
        void ClearLocation(Vec2 _pos); 
        void OpenDoor (Vec2 _pos);
        Monster* GetMonsterAt(Vec2 _pos);
        void RemoveMonster(Monster* _monster);
        bool HasMonsters() const { return !m_monsters.empty(); }
        void AddThirdLevelTreasureDoor(); 
        void LoadTreasureRoom(); 
        bool IsTreasureRoom() const { return m_isTreasureRoom; }

    private: 
        vector<Entity*> m_entities; 
        Player* m_player = nullptr; 
        vector<Monster*> m_monsters; 
        vector<vector<char>> m_map; 
        vector<Door> m_doors; // Tracks whether the current room is the special treasure room 
        bool m_isTreasureRoom = false; // Counts how many room loads have happened so we can trigger the door every 3rd one 
        static int s_levelLoopCount; 
};

