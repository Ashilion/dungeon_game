#ifndef WIN_VIEW_H
#define WIN_VIEW_H

#include "View.h"
#include <SFML/Graphics.hpp>

class WinView : public View {
private:
    sf::Text winMessage;

public:
    WinView(sf::RenderWindow& window, const sf::Font& font);
    void update() override;
    void draw() override;
};

#endif // WIN_VIEW_H