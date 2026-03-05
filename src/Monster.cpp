#include "Monster.hpp"
#include "Room.hpp"

void Monster::Start(Vec2 _pos) {
    m_character = 'E';
    m_position = _pos;
}

void Monster::Update() {
    // Monster does nothing for now
}