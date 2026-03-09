#include "Room.hpp"
using namespace std; 
#include "Player.hpp"
#include "Monster.hpp"

// global counter to give each monster a unique label
static int s_monsterCount = 0;

#include <fstream>
#include <string>
#include <algorithm>

void Room::Load(string _path)
{
    m_map.clear();
    m_doors.clear();

    ifstream file;
    file.open(_path);

    if (!file.is_open())
    {
        printf("file not found at: %s \n", _path.c_str());
        exit(1);
    }

    string word;
    int number;

    while (file >> word)
    {
        if (word == "level")
        {
            if (file >> number)
            {
                printf("open level: %i\n", number);
            }
        }

        if (word == "next_level")
        {
            if (file >> word)
            {
                m_doors.push_back(Door());
                m_doors[m_doors.size() - 1].path = word;
            }
        }

        if (word == "map")
        {
            m_map.push_back(vector<char>());
            while(file >> word)
            {
                if (word == "-2")
                {
                    break;
                }

                if (word == "-1")
                {
                    m_map.push_back(vector<char>());
                    continue;
                }

                if (word == "0")
                    m_map[m_map.size() - 1].push_back(' ');
                else
                    m_map[m_map.size() - 1].push_back(word[0]);
                
            }
        }
    }

    int doorCount = 0;
    for (int y = 0; y < m_map.size(); y++)
    {
        for (int x = 0; x < m_map[y].size(); x++)
        {
            if (m_map[y][x] == 'S')
            {
                if (m_player == nullptr)
                    m_player = new Player();
                
                m_player->Start(Vec2(x,y));
                m_map[y][x] = ' ';
            }

            if (m_map[y][x] == 'E')
            {
                MonsterType t = (rand() % 2 == 0) ? MonsterType::Goblin : MonsterType::Orc;
                Monster* monster = new Monster(t);
                // give it a unique number so labels differ
                monster->name += " #" + to_string(++s_monsterCount);
                monster->Start(Vec2(x,y));
                m_monsters.push_back(monster);
                m_map[y][x] = ' ';
            }

            if (m_map[y][x] == 'D' || m_map[y][x] == 'L')
            {
                if (m_doors.size() - 1 >= doorCount)
                {
                    m_doors[doorCount].pos.x = x;
                    m_doors[doorCount].pos.y = y;
                    doorCount++;
                }
            }
        }
    }

    // Lock doors if monsters are present
    if (!m_monsters.empty()) 
    {
        for (auto& row : m_map) 
        {
            for (auto& cell : row) 
            {
                if (cell == 'D') 
                {
                    cell = 'L';  // Lock open doors
                }
            }
        }
    }
}

void Room::Update()
{
    Draw();
    if (m_player != nullptr)
    {
        m_player->room = this;
        m_player->Update();
    }
    for (auto monster : m_monsters)
    {
        monster->room = this;
        monster->Update();
    }
}

void Room::Draw()
{
    for (int y = 0; y < m_map.size(); y++)
    {
        for (int x = 0; x < m_map[y].size(); x++)
        {
            printf("%c ", GetLocation(Vec2(x, y)));
        }
        printf("\n");
    }
}

char Room::GetLocation(Vec2 _pos)
{
    if (_pos.y >= m_map.size())
        return ' ';
    
    if (_pos.x >= m_map[_pos.y].size())
        return ' ';

    if (m_player != nullptr)
        if (m_player->GetPosition() == _pos)
            return m_player->Draw();
    
    for (auto monster : m_monsters)
    {
        if (monster->GetPosition() == _pos)
            return monster->Draw();
    }
    
    return m_map[_pos.y][_pos.x];
}

void Room::ClearLocation(Vec2 _pos)
{
    if (_pos.y >= m_map.size())
        return;
    
    if (_pos.x >= m_map[_pos.y].size())
        return;
    
    m_map[_pos.y][_pos.x] = ' ';
}

void Room::OpenDoor(Vec2 _pos)
{
    for(int i = 0; i < m_doors.size(); i++)
    {
        if (m_doors[i].pos == _pos)
        {
            if (m_player != nullptr)
            {
                m_player->Heal(3); // Heal 3 health when entering a new room
            }
            Load(m_doors[i].path.c_str());
        }
    }
}

Monster* Room::GetMonsterAt(Vec2 _pos)
{
    for (auto monster : m_monsters)
    {
        if (monster->GetPosition() == _pos)
            return monster;
    }
    return nullptr;
}

void Room::RemoveMonster(Monster* _monster)
{
    auto it = find(m_monsters.begin(), m_monsters.end(), _monster);
    if (it != m_monsters.end())
    {
        m_monsters.erase(it);
        delete _monster;
    }

    // Unlock doors if no monsters left
    if (m_monsters.empty()) 
    {
        for (auto& row : m_map) 
        {
            for (auto& cell : row) 
            {
                if (cell == 'L') 
                {
                    cell = 'D';  // Unlock doors
                }
            }
        }
    }
}