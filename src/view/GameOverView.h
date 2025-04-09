#ifndef GAME_OVER_VIEW_H
#define GAME_OVER_VIEW_H

#include "View.h"
#include <SFML/Graphics.hpp>

class GameOverView : public View {
private:
    sf::Text gameOverMessage;

public:
    GameOverView(sf::RenderWindow& window, const sf::Font& font);
    void update() override;
    void draw() override;
};

#endif // GAME_OVER_VIEW_H