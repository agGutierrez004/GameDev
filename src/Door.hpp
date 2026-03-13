#pragma once
#include <string>
#include "fogpi/Math.hpp"
using namespace std; 
struct Door
{
    Vec2 pos;
    string path;
    bool requiresKey = false; 
};