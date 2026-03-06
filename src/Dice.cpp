#include "Dice.hpp"
using namespace std;
RollStats RollDice(vector<Die> _dice)
{
    RollStats stats = {};

    for (Die d : _dice)
    {
        stats.min += 1;
        stats.max += d.sides;

        int roll = random_int(1, d.sides);//1 + rand() % d.sides;

        stats.total += roll;

        if (roll > stats.greatest)
            stats.greatest = roll;
        
        if (roll == d.sides)
            stats.critCount++;
    }

    return stats;
}