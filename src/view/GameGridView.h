#pragma once

#include "View.h"
#include "../Entity.h"
#include <vector>
#include <unordered_map>

class GameGridView : public View {
public:
    GameGridView(sf::RenderWindow& window, 
                std::vector<std::vector<int>>& grid,
                 const Entity& player,
                 const std::vector<Entity>& enemies);
    
    void draw() override;
    void update() override;
    
    void setGrid(const std::vector<std::vector<int>>& grid);
    void setVisionRadius(int radius);
    void loadTextures();
    
private:
    std::vector<std::vector<int>>& grid;
    const Entity& player;
    const std::vector<Entity>& enemies;
    int visionRadius;
    
    std::unordered_map<std::string, sf::Texture> textures;
    
    sf::Sprite createSprite(const sf::Texture& texture);
};