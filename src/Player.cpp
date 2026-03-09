#include "Player.hpp"
#include "Room.hpp"
#include "Monster.hpp"
#include "Dice.hpp"
#include "fogpi/io.hpp"
using namespace std; 
void Player::Start(Vec2 _pos) {
    m_character = 'P';
    m_position = _pos;
}

void Player::Death(){
    printf("Player has died with %d coins", coins);
    exit(0);
}
void Player::Update() {
    //while(request_char("hit w to continue: ") != 'w') {}

    char directionInput;

    do {
        directionInput = request_char("wasd and Enter to move");
    } while (directionInput != 'w' &&
             directionInput != 'a' &&
             directionInput != 's' &&
             directionInput != 'd');
    
    Vec2 direction(0.0f);

    switch (directionInput)
    {
    case 'w':
        direction = {0.0f, -1.0f};
        break;
    case 'a':
        direction = {-1.0f, 0.0f};
        break;
    case 's':
        direction = {0.0f, 1.0f};
        break;
    case 'd':
        direction = {1.0f, 0.0f};
        break;
    default:
        direction = {0.0f, 1.0f};
        break;
    }

    Vec2 tryPos = m_position + direction;

    Monster* monster = room->GetMonsterAt(tryPos);
    if (monster != nullptr)
    {
        // Fight
        vector<Die> playerDice = { {6} };
        vector<Die> monsterDice = { {6} };
        RollStats playerRoll = RollDice(playerDice);
        RollStats monsterRoll = RollDice(monsterDice);
        printf("Player rolls %d, Monster rolls %d\n", playerRoll.total, monsterRoll.total);
        if (playerRoll.total > monsterRoll.total)
        {
            printf("Player wins! Monster defeated.\n");
            room->RemoveMonster(monster);
        }
        else
        {
            printf("Monster wins! Player takes damage.\n");
            health -= 2;
            if (health <= 0)//player dies 
            {
                Player::Death();
            }
        }
        PrintStats();
        return; // Don't move
    }

    char loc = room->GetLocation(tryPos);
    if (loc == 'K') {
        m_keyCount++;
        room->ClearLocation(tryPos);
    }
    else if (loc == 'C') {
        coins++;
        room->ClearLocation(tryPos);
    }

    if (loc == ' ') {
        m_position = tryPos;
    }

    if (loc == 'D') {
        room->OpenDoor(tryPos);
    } else if (loc == 'L') {
        if (!room->HasMonsters()) {
            room->OpenDoor(tryPos);
        } else {
            printf("The door is locked until all monsters are defeated!\n");
        }
    }
}

void Player::PrintStats() {
    printf("\n=== Player Stats ===\n");
    printf("Health: %d\n", health);
    printf("Keys: %d\n", m_keyCount);
    printf("Coins: %d\n", coins);
    printf("====================\n\n");
}



void Player::Heal(int amount) {
    int oldHealth = health;
    health += amount;
    if (health > 10) {
        health = 10; // Max health is 10
    }
    printf("\nRestored in new room! Healed %d health (was %d, now %d)\n\n", health - oldHealth, oldHealth, health);
}