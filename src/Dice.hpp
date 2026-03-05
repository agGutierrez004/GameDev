#pragma once

#include "fogpi/io.hpp"
#include <vector>

struct Die
{
    int sides = 20;
};

struct RollStats
{
    int min = 0;
    int max = 0;
    int greatest = 0;
    int critCount = 0;
    int total = 0;
};

RollStats RollDice(std::vector<Die> _dice);

