#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <random> // Add this include for std::mt19937
#include "Object.h"
#include <iostream>

class Weapon : public Object {
private:
    int attackValue;

public:
    Weapon(const sf::Vector2i& pos, int value, const std::string& texturePath = "assets/weapon.png");

    // Override use method to increase attack
    bool use(Entity& entity) override;

    // Get the attack value
    int getAttackValue() const;

    // Override description for more detail
    std::string getDescription() const override;

    static void generateWeapons(std::vector<std::vector<int>>& grid, 
        std::vector<Object*>& worldObjects, 
        int itemCount, int level, std::mt19937& rng);
};