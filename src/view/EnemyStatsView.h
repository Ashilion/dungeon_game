#pragma once

#include "View.h"
#include "../Entity.h"
#include <vector>
#include <string>

struct EnemyType {
    int baseHp;
    int baseAttack;
    sf::Color color;
    std::string name;
};

class EnemyStatsView : public View {
public:
    EnemyStatsView(sf::RenderWindow& window, 
                   const Entity& player,
                   const std::vector<Entity>& enemies,
                   const std::vector<EnemyType>& enemyTypes);
    
    void draw() override;
    void update() override;
    
    void setInCombat(bool inCombat);
    
private:
    const Entity& player;
    const std::vector<Entity>& enemies;
    const std::vector<EnemyType>& enemyTypes;
    bool inCombat;
    sf::Font font;
    sf::Text statsText;
    sf::RectangleShape background;
};