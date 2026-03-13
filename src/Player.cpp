#include "Player.hpp"
#include "Room.hpp"
#include "Monster.hpp"
#include "Dice.hpp"
#include "fogpi/io.hpp"
#include <algorithm>
#include <random>
using namespace std;

void Player::Start(Vec2 _pos) {
    m_character = 'P';
    m_position = _pos;
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
        vector<Die> playerDice(m_attackDice, Die{6});
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
        printf("You picked up a key!\n");
        room->ClearLocation(tryPos);
    }
    else if (loc == 'C') {
        int coinAmount = random_int(1, 3);
        coins += coinAmount;
        printf("You picked up %d coin(s)!\n", coinAmount);
        room->ClearLocation(tryPos);
    }
    else if (loc == 'H') {
        int healAmount = random_int(3, 6);
        health += healAmount;
        if (health > maxHealth) {
            health = maxHealth;
        }

        printf("You picked up a health potion and healed %d HP!\n", healAmount);
        room->ClearLocation(tryPos);
    }
    
    else if (loc == 'G') {
        int goldAmount = 5;
        coins += goldAmount;
        printf("You opened the chest and found %d gold!\n", goldAmount);
        room->ClearLocation(tryPos);
    }

    if (loc == ' ') {
        m_position = tryPos;
    }
    else if (loc == 'K' || loc == 'C' || loc == 'G' || loc == 'H') {
        m_position = tryPos;
    }

    if (loc == 'D') {
        room->OpenDoor(tryPos);
    } 
    else if (loc == 'L') {
        if (!room->HasMonsters()) {
            room->OpenDoor(tryPos);
        } else {
            printf("The door is locked until all monsters are defeated!\n");
        }
    }
    else if (loc == 'T') {
        room->OpenDoor(tryPos);
    }
}

void Player::PrintStats() {
    printf("\n=== Player Stats ===\n");
    printf("Health: %d\n", health);
    printf("Keys: %d\n", m_keyCount);
    printf("Coins: %d\n", coins);
    printf("Attack dice: %d\n", m_attackDice);
    if (coinBonus)
        printf("Coin bonus: +%d\n", coinBonus);
    printf("====================\n\n");
}

void Player::Heal(int amount) {
    int oldHealth = health;
    health += amount;
    if (health > maxHealth) {
        health = maxHealth;
    }
    printf("\nRestored in new room! Healed %d health (was %d, now %d)\n\n", health - oldHealth, oldHealth, health);
}

const std::vector<Player::Upgrade>& Player::GetAllUpgrades()
{
    static std::vector<Upgrade> s_upgrades = {
        { "Max health +2", /*cost*/ 2, [](Player& p) {
              p.maxHealth += 2;
              p.health = std::min(p.health + 2, p.maxHealth);
          } },
        { "Extra attack die", /*cost*/ 4, [](Player& p) {
              p.m_attackDice += 1;
          } },
        { "Extra Coins on Pickup", /*cost*/ 3, [](Player& p) {
              p.coinBonus += 1;
          } },
    };
    return s_upgrades;
}

void Player::OfferUpgrades()
{
    auto all = GetAllUpgrades();
    if (all.empty())
        return;

    std::vector<Upgrade> choices;
    std::sample(all.begin(), all.end(),
                std::back_inserter(choices),
                std::min<size_t>(3, all.size()),
                std::mt19937{std::random_device{}()});

    printf("\n--- Choose an upgrade ---\n");
    printf("0) None\n");
    for (size_t i = 0; i < choices.size(); ++i)
        printf("%zu) %s (cost %d coins)\n", i + 1,
               choices[i].name.c_str(), choices[i].cost);

    char c;
    int idx;
    std::string prompt = "pick 0-" + std::to_string(choices.size()) + ": ";
    do {
        c = request_char(prompt.c_str());
        if (c == '0') {
            printf("No upgrade taken.\n\n");
            return; // exit early, player declines
        }
        if (c < '1' || c >= '1' + (int)choices.size())
            continue;
        idx = c - '1';
        if (coins < choices[idx].cost) {
            printf("Not enough coins! You have %d, need %d.\n",
                   coins, choices[idx].cost);
            c = 0; // force loop again
        }
    } while (c == 0);

    coins -= choices[idx].cost;
    choices[idx].apply(*this);

    printf("You picked: %s\n\n", choices[idx].name.c_str());
    request_char("press enter to continue");
}

void Player::Death(){
    printf("Player has died with %d coins\n", coins);
    printf("For a death anthem:\n");
    printf("https://www.youtube.com/watch?v=dQw4w9WgXcQ\n");
    exit(0);
}