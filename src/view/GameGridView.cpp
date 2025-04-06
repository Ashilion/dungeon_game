#include "GameGridView.h"
#include <iostream>
#include <cmath>
#include "../Maze.h"

GameGridView::GameGridView(sf::RenderWindow& window, 
                            std::vector<std::vector<int>>& grid,
                           const Entity& player,
                           const std::vector<Entity>& enemies)
    : View(window), grid(grid), player(player), enemies(enemies), visionRadius(8) {
    
    // Set the size based on the grid dimensions
    if (!grid.empty()) {
        setSize(sf::Vector2f(grid[0].size() * GRID_SIZE, grid.size() * GRID_SIZE));
    }
    
    loadTextures();
}

void GameGridView::loadTextures() {
    // Load all the necessary textures
    sf::Texture texture;
    
    if (texture.loadFromFile("assets/player2.png")) {
        textures["player"] = texture;
    } else {
        std::cerr << "Error loading player texture\n";
    }
    
    if (texture.loadFromFile("assets/wall.jpg")) {
        textures["wall"] = texture;
    } else {
        std::cerr << "Error loading wall texture\n";
    }
    
    if (texture.loadFromFile("assets/stairs.png")) {
        textures["stairs"] = texture;
    } else {
        std::cerr << "Error loading stairs texture\n";
    }
    
    if (texture.loadFromFile("assets/heal.png")) {
        textures["heal"] = texture;
    } else {
        std::cerr << "Error loading heal texture\n";
    }
    
    if (texture.loadFromFile("assets/weapon.png")) {
        textures["weapon"] = texture;
    } else {
        std::cerr << "Error loading weapon texture\n";
    }
    
    if (texture.loadFromFile("assets/armor.png")) {
        textures["armor"] = texture;
    } else {
        std::cerr << "Error loading armor texture\n";
    }
    
    if (texture.loadFromFile("assets/floor.jpg")) {
        textures["floor"] = texture;
    } else {
        std::cerr << "Error loading floor texture\n";
    }
    
    if (texture.loadFromFile("assets/treasure.jpg")) {
        textures["treasure"] = texture;
    } else {
        std::cerr << "Error loading treasure texture\n";
    }
    
    // Load enemy textures
    if (texture.loadFromFile("assets/goblin.png")) {
        textures["enemy_0"] = texture;
    } else {
        std::cerr << "Error loading goblin texture\n";
    }
    
    if (texture.loadFromFile("assets/orc.png")) {
        textures["enemy_1"] = texture;
    } else {
        std::cerr << "Error loading orc texture\n";
    }
    
    if (texture.loadFromFile("assets/troll.png")) {
        textures["enemy_2"] = texture;
    } else {
        std::cerr << "Error loading troll texture\n";
    }
}

sf::Sprite GameGridView::createSprite(const sf::Texture& texture) {
    sf::Sprite sprite(texture);
    
    // Get the texture size
    sf::Vector2u textureSize = texture.getSize();
    
    // Calculate scale to make sprite exactly GRID_SIZE pixels in both dimensions
    float scaleX = static_cast<float>(GRID_SIZE) / textureSize.x;
    float scaleY = static_cast<float>(GRID_SIZE) / textureSize.y;
    
    // Set the calculated scale
    sprite.setScale(sf::Vector2f(scaleX, scaleY));
    
    return sprite;
}

void GameGridView::draw() {
    if (!visible || grid.empty()) return;
    
    // Draw the floor and game elements with fog of war effect
    for (size_t y = 0; y < grid.size(); y++) {
        for (size_t x = 0; x < grid[y].size(); x++) {
            // Calculate Manhattan distance between the cell and player
            int dx = std::abs(static_cast<int>(x) - player.pos.x);
            int dy = std::abs(static_cast<int>(y) - player.pos.y);
            float distance = static_cast<float>(dx + dy);
            
            // Apply progressive transparency based on distance
            float alpha = 255.f; // Full opacity by default
            if (distance > visionRadius / 2) {
                // Reduce opacity progressively between half radius and edge
                alpha = 255.f * (visionRadius - distance) / (visionRadius / 2);
                alpha = std::max(0.f, std::min(255.f, alpha)); // Clamp between 0 and 255
            }
            
            // Skip rendering completely invisible tiles for optimization
            if (alpha <= 5.0f) continue;
            
            sf::Sprite sprite(textures["floor"]); // Forced to create with a texture
            
            switch (grid[y][x]) {
                case EMPTY:
                    sprite = createSprite(textures["floor"]);
                    break;
                case WALL:
                    sprite = createSprite(textures["wall"]);
                    break;
                case PLAYER:
                    // Draw floor under player
                    sprite = createSprite(textures["floor"]);
                    sprite.setPosition(sf::Vector2f(x * GRID_SIZE, y * GRID_SIZE));
                    sprite.setColor(sf::Color(255, 255, 255, static_cast<uint8_t>(alpha)));
                    window.draw(sprite);
                    // Draw player
                    sprite = createSprite(textures["player"]);
                    break;
                case STAIRS:
                    sprite = createSprite(textures["stairs"]);
                    break;
                case ENEMY:
                    // Find the enemy type at this position
                    for (const auto& enemy : enemies) {
                        if (enemy.pos.x == static_cast<int>(x) && enemy.pos.y == static_cast<int>(y)) {
                            std::string enemyTexture = "enemy_" + std::to_string(enemy.type);
                            if (textures.find(enemyTexture) != textures.end()) {
                                sprite = createSprite(textures[enemyTexture]);
                            } else {
                                // Fallback to a generic enemy texture
                                sprite = createSprite(textures["enemy_0"]);
                            }
                            break;
                        }
                    }
                    break;
                case HEAL:
                    sprite = createSprite(textures["heal"]);
                    break;
                case WEAPON:
                    sprite = createSprite(textures["weapon"]);
                    break;
                case ARMOR:
                    sprite = createSprite(textures["armor"]);
                    break;
                case ROOM:
                    sprite = createSprite(textures["floor"]);
                    break;
                case TREASURE:
                    sprite = createSprite(textures["treasure"]);
                    break;
                default:
                    sprite = createSprite(textures["floor"]);
                    break;
            }
            
            // Apply position and transparency
            sprite.setPosition(sf::Vector2f(x * GRID_SIZE, y * GRID_SIZE));
            sprite.setColor(sf::Color(255, 255, 255, static_cast<uint8_t>(alpha)));
            window.draw(sprite);
        }
    }
}

void GameGridView::update() {
    // Not much to update for the grid view itself
}

void GameGridView::setGrid(const std::vector<std::vector<int>>& newGrid) {
    grid = newGrid;
    if (!grid.empty()) {
        setSize(sf::Vector2f(grid[0].size() * GRID_SIZE, grid.size() * GRID_SIZE));
    }
}

void GameGridView::setVisionRadius(int radius) {
    visionRadius = radius;
}