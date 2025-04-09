#include "HealPotion.h"
#include "Object.h"
#include "Entity.h"
#include "Maze.h"
#include <iostream>
// ----------------- HealPotion Implementation -----------------

HealPotion::HealPotion(const sf::Vector2i& pos, int amount, const std::string& texturePath) 
    : Object(pos, "Healing Potion", amount, texturePath), healAmount(amount) {}

bool HealPotion::use(Entity& entity) {
    if (collected) {
        entity.hp += healAmount;
        return true; // Successfully used
    }
    return false; // Not collected yet
}

int HealPotion::getHealAmount() const { 
    return healAmount; 
}

std::string HealPotion::getDescription() const {
    return name + " (+" + std::to_string(healAmount) + " HP)";
}

void HealPotion::generateHealPotions(std::vector<std::vector<int>>& grid, 
    std::vector<Object*>& worldObjects, 
    int itemCount, int level, std::mt19937& rng) {
for (int i = 0; i < itemCount; i++) {
sf::Vector2i itemPos;
do {
itemPos.x = std::uniform_int_distribution<int>(1, GRID_WIDTH - 2)(rng);
itemPos.y = std::uniform_int_distribution<int>(1, GRID_HEIGHT - 2)(rng);
} while (grid[itemPos.y][itemPos.x] != EMPTY);

grid[itemPos.y][itemPos.x] = HEAL;

int healAmount = 20 + level * 5;
HealPotion* potion = new HealPotion(itemPos, healAmount, "assets/heal.png");
worldObjects.push_back(potion);
}
}