#include "GameOverView.h"

GameOverView::GameOverView(sf::RenderWindow& window, const sf::Font& font) : View(window), gameOverMessage(font) {
    gameOverMessage.setFont(font);
    gameOverMessage.setCharacterSize(32);
    gameOverMessage.setString("Game Over! Try again.");
    gameOverMessage.setFillColor(sf::Color::Red);
    gameOverMessage.setPosition(sf::Vector2f(window.getSize().x / 2 - 100, window.getSize().y / 2));
    this->visible = false;
}   

void GameOverView::update() {
    // Update logic if needed
}

void GameOverView::draw() {
    window.draw(gameOverMessage);
}