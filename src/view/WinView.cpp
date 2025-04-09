#include "WinView.h"
#include <iostream>

WinView::WinView(sf::RenderWindow& window, const sf::Font& font) : View(window), winMessage(font) {
    winMessage.setFont(font);
    winMessage.setCharacterSize(32);
    winMessage.setString("Congratulations! You won the game!");
    winMessage.setFillColor(sf::Color::Green);
    winMessage.setPosition(sf::Vector2f(window.getSize().x / 2 - 150, window.getSize().y / 2));
    this->visible = false;
}

void WinView::update() {
    // Update logic if needed
}

void WinView::draw() {
    // Clear the window with a solid color for the win screen
    window.clear(sf::Color::Black);

    // Draw the win message
    window.draw(winMessage);
}