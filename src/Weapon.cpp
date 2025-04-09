#include "Weapon.h"
#include "Entity.h"
#include "Maze.h"

Weapon::Weapon(const sf::Vector2i& pos, int value, const std::string& texturePath) 
    : Object(pos, "Weapon", value, texturePath), attackValue(value) {}

bool Weapon::use(Entity& entity) {
    if (collected) {
        entity.attack += attackValue; // Increase attack power
        return true; // Successfully used
    }
    return false; // Not collected yet
}

int Weapon::getAttackValue() const {
    return attackValue;
}

std::string Weapon::getDescription() const {
    return name + " (Attack: +" + std::to_string(attackValue) + ")";
}

void Weapon::generateWeapons(std::vector<std::vector<int>>& grid, 
    std::vector<Object*>& worldObjects, 
    int itemCount, int level, std::mt19937& rng) {
    for (int i = 0; i < itemCount; i++) {
        sf::Vector2i itemPos;
        do {
            itemPos.x = std::uniform_int_distribution<int>(1, GRID_WIDTH - 2)(rng);
            itemPos.y = std::uniform_int_distribution<int>(1, GRID_HEIGHT - 2)(rng);
        } while (grid[itemPos.y][itemPos.x] != EMPTY);

        grid[itemPos.y][itemPos.x] = WEAPON;

        int attackValue = 15 + level * 4;
        Weapon* weapon = new Weapon(itemPos, attackValue, "assets/weapon.png");
        worldObjects.push_back(weapon);
    }
}