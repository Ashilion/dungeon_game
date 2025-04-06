#pragma once

#include "View.h"
#include "../Entity.h"

class PlayerStatsView : public View {
public:
    PlayerStatsView(sf::RenderWindow& window, const Entity& player);
    
    void draw() override;
    void update() override;
    
    void setLevel(int level);
    void setSkillPoints(int points);
    
private:
    const Entity& player;
    int level;
    int skillPoints;
    sf::Font font;
    sf::Text statsText;
    sf::RectangleShape background;
};