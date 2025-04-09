#include "Key.h"
#include "Maze.h"
#include <iostream>

Key::Key(const sf::Vector2i& pos, const std::string& texturePath)
    : Object(pos, "Key", 0, texturePath) {} 

std::string Key::getDescription() const {
    return "A key to open the treasure chest.";
}

bool Key::use(Entity& entity) {
    if (collected) {
        std::cout << "The key is used to open the treasure chest!" << std::endl;
        return true; // Successfully used
    }
    return false; // Not collected yet
}

void Key::generateKey(std::vector<std::vector<int>>& grid, std::vector<Object*>& worldObjects, std::mt19937& rng) {
    sf::Vector2i keyPos;
    do {
        keyPos.x = std::uniform_int_distribution<int>(1, GRID_WIDTH - 2)(rng);
        keyPos.y = std::uniform_int_distribution<int>(1, GRID_HEIGHT - 2)(rng);
    } while (grid[keyPos.y][keyPos.x] != EMPTY);

    std::cout << "Placing key at position: (" << keyPos.x << ", " << keyPos.y << ")" << std::endl;

    grid[keyPos.y][keyPos.x] = KEY;
    Key* key = new Key(keyPos, "assets/key.jpg");
    worldObjects.push_back(key);

    std::cout << "Key successfully added to worldObjects." << std::endl;
}

