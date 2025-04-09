#include "Armor.h"
#include "Entity.h"
#include "Maze.h"
#include <random>
#include <iostream>

Armor::Armor(const sf::Vector2i& pos, int value, const std::string& texturePath) 
    : Object(pos, "Armor", value, texturePath), defenseValue(value) {}

bool Armor::use(Entity& entity) {
    if (collected) {
        entity.hp += defenseValue; // Increase HP as a form of defense
        return true; // Successfully used
    }
    return false; // Not collected yet
}

int Armor::getDefenseValue() const {
    return defenseValue;
}

std::string Armor::getDescription() const {
    return name + " (Defense: +" + std::to_string(defenseValue) + " HP)";
}

void Armor::generateArmors(std::vector<std::vector<int>>& grid, 
    std::vector<Object*>& worldObjects, 
    int itemCount, int level, std::mt19937& rng) {
    for (int i = 0; i < itemCount; i++) {
        sf::Vector2i itemPos;
        do {
            itemPos.x = std::uniform_int_distribution<int>(1, GRID_WIDTH - 2)(rng);
            itemPos.y = std::uniform_int_distribution<int>(1, GRID_HEIGHT - 2)(rng);
        } while (grid[itemPos.y][itemPos.x] != EMPTY);

        grid[itemPos.y][itemPos.x] = ARMOR;

        int defenseValue = 10 + level * 3;
        Armor* armor = new Armor(itemPos, defenseValue, "assets/armor.png");
        worldObjects.push_back(armor);
    }
}