#ifndef WALL_BREAKER_VIEW_H
#define WALL_BREAKER_VIEW_H

#include "View.h"
#include <SFML/Graphics.hpp>
#include "../Entity.h"

class WallBreakerView : public View {
private:
    sf::RectangleShape highlight;
    sf::Text instructions;
    sf::Clock wallBreakerClock;
    const Entity& player;
    std::vector<std::vector<int>>& grid;

public:
    WallBreakerView(sf::RenderWindow& window, const sf::Font& font,const Entity& player,std::vector<std::vector<int>>& grid);
    void update() override;
    void draw() override;
    void setHighlightPosition(const sf::Vector2f& position);
};

#endif // WALL_BREAKER_VIEW_H