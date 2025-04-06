#pragma once
#include <SFML/Graphics.hpp>

struct Entity {
    sf::Vector2i pos;
    int hp;
    int attack;
    int type;  // Used for enemies to determine their type
    Entity(sf::Vector2i p, int h, int a, int t = 0) : pos(p), hp(h), attack(a), type(t) {}
};