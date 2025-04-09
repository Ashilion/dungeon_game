#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include "Object.h"
#include "random"

class Armor : public Object {
private:
    int defenseValue;

public:
    Armor(const sf::Vector2i& pos, int value, const std::string& texturePath = "assets/armor.png");

    // Override use method to increase defense
    bool use(Entity& entity) override;

    // Get the defense value
    int getDefenseValue() const;

    // Override description for more detail
    std::string getDescription() const override;

    static void generateArmors(std::vector<std::vector<int>>& grid, 
        std::vector<Object*>& worldObjects, 
        int itemCount, int level, std::mt19937& rng);
};