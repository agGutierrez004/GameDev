#include "Monster.hpp"
#include "Room.hpp"

void Monster::Start(Vec2 _pos) {
    // character already set by constructor based on type
    m_position = _pos;
}

void Monster::Update() {
    // simple debug output for this monster
    cout << "[Monster] ";
    display();
    // behaviour to be implemented later
}