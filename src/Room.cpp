#include "Room.hpp"
#include "Player.hpp"
#include "Monster.hpp"
#include "Dice.hpp"

#include <fstream>
#include <string>
#include <algorithm>
#include <cstdio>
#include <cstdlib>

using namespace std;

// global counter to give each monster a unique label
static int s_monsterCount = 0;

// counts room loads so the treasure door appears every 3rd level
int Room::s_levelLoopCount = 0;

void Room::Load(string _path)
{
    m_map.clear();
    m_doors.clear();
    m_monsters.clear();
    m_isTreasureRoom = false;

    // count each room load
    s_levelLoopCount++;

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
                Door newDoor; 
                newDoor.path = word;
                m_doors.push_back(newDoor);
            }
        }

        if (word == "map")
        {
            m_map.push_back(vector<char>());

            while (file >> word)
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
    for (int y = 0; y < (int)m_map.size(); y++)
    {
        for (int x = 0; x < (int)m_map[y].size(); x++)
        {
            if (m_map[y][x] == 'S')
            {
                if (m_player == nullptr)
                    m_player = new Player();

                m_player->Start(Vec2(x, y));
                m_map[y][x] = ' ';
            }

            if (m_map[y][x] == 'E')
            {
                MonsterType t = (rand() % 2 == 0) ? MonsterType::Goblin : MonsterType::Orc;
                Monster* monster = new Monster(t);

                // give it a unique number so labels differ
                monster->name += " #" + to_string(++s_monsterCount);
                monster->Start(Vec2(x, y));
                m_monsters.push_back(monster);

                m_map[y][x] = ' ';
            }

            if (m_map[y][x] == 'D' || m_map[y][x] == 'L')
            {
                if (doorCount < (int)m_doors.size())
                {
                    m_doors[doorCount].pos = Vec2(x, y);
                    doorCount++;
                }
            }
        }
    }

    // Lock normal doors if monsters are present
    if (!m_monsters.empty())
    {
        for (auto& row : m_map)
        {
            for (auto& cell : row)
            {
                if (cell == 'D')
                {
                    cell = 'L';
                }
            }
        }
    }

    // Add the special locked treasure door every 3rd level load
    // but only for normal rooms, not inside the treasure room
    if (s_levelLoopCount % 3 == 0)
    {
        AddThirdLevelTreasureDoor();
    }
}

void Room::AddThirdLevelTreasureDoor()
{
    // find an empty tile and place the treasure door there
    for (int y = 0; y < (int)m_map.size(); y++)
    {
        for (int x = 0; x < (int)m_map[y].size(); x++)
        {
            if (m_map[y][x] == ' ')
            {
                m_map[y][x] = 'T';   // T = locked treasure door

                Door treasureDoor;
                treasureDoor.pos = Vec2(x, y);
                treasureDoor.path = "TREASURE_ROOM";
                treasureDoor.requiresKey = true;

                m_doors.push_back(treasureDoor);

                printf("A locked treasure door has appeared on this level!\n");
                return;
            }
        }
    }
}

void Room::LoadTreasureRoom()
{
    m_map.clear();
    m_doors.clear();
    m_monsters.clear();
    m_isTreasureRoom = true;

    // simple treasure room
    m_map = {
        {'#', '#', '#', '#', '#'},
        {'#', ' ', ' ', ' ', '#'},
        {'#', ' ', 'G', ' ', '#'},
        {'#', ' ', ' ', ' ', '#'},
        {'#', '#', '#', '#', '#'}
    };

    if (m_player == nullptr)
        m_player = new Player();

    // place player near the chest
    m_player->Start(Vec2(2, 3));

    printf("You entered the treasure room!\n");
}

void Room::Draw()
{
    for (int y = 0; y < (int)m_map.size(); y++)
    {
        for (int x = 0; x < (int)m_map[y].size(); x++)
        {
            printf("%c ", GetLocation(Vec2(x, y)));
        }
        printf("\n");
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

char Room::GetLocation(Vec2 _pos)
{
    if (_pos.y < 0 || _pos.y >= (int)m_map.size())
        return ' ';

    if (_pos.x < 0 || _pos.x >= (int)m_map[_pos.y].size())
        return ' ';

    if (m_player != nullptr && m_player->GetPosition() == _pos)
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
    if (_pos.y < 0 || _pos.y >= (int)m_map.size())
        return;

    if (_pos.x < 0 || _pos.x >= (int)m_map[_pos.y].size())
        return;

    m_map[_pos.y][_pos.x] = ' ';
}

void Room::OpenDoor(Vec2 _pos)
{
    for (int i = 0; i < (int)m_doors.size(); i++)
    {
        if (m_doors[i].pos == _pos)
        {
            // special treasure door: needs a key
            if (m_doors[i].requiresKey)
            {
                if (m_player == nullptr)
                    return;

                if (m_player->m_keyCount <= 0)
                {
                    printf("This treasure door is locked. You need a key.\n");
                    return;
                }

                m_player->m_keyCount--;
                printf("You used 1 key to unlock the treasure door.\n");
                LoadTreasureRoom();
                return;
            }

            // normal door behavior
            if (m_player != nullptr)
            {
                m_player->Heal(3);   // heal 3 health when entering a new room
                m_player->OfferUpgrades();
            }

            Load(m_doors[i].path.c_str());
            return;
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
    if (_monster == nullptr)
        return;

    // Save the monster's position before deleting it
    Vec2 dropPos = _monster->GetPosition();

    auto it = find(m_monsters.begin(), m_monsters.end(), _monster);
    if (it != m_monsters.end())
    {
        m_monsters.erase(it);
        delete _monster;
    }

    // 50/50 chance:
    // H = potion that heals 3-6
    // C = coins that give 1-3 when picked up
    if (dropPos.y >= 0 && dropPos.y < (int)m_map.size() &&
        dropPos.x >= 0 && dropPos.x < (int)m_map[dropPos.y].size() &&
        m_map[dropPos.y][dropPos.x] == ' ')
    {
    int drop = random_int(0, 3);
        if (drop == 0)
        {
            m_map[dropPos.y][dropPos.x] = 'H';
            printf("The monster dropped a health potion!\n");
        }
        else if (drop == 2)
        {
            m_map[dropPos.y][dropPos.x] = 'C';
            printf("The monster dropped some coins!\n");
        }
        
    }

    // unlock normal doors if no monsters left
    if (m_monsters.empty())
    {
        for (auto& row : m_map)
        {
            for (auto& cell : row)
            {
                if (cell == 'L')
                {
                    cell = 'D';
                }
            }
        }
    }
}