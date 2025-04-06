#include "View.h"

View::View(sf::RenderWindow& window, sf::Vector2f position, sf::Vector2f size)
    : window(window), position(position), size(size), visible(true) {
}

bool View::handleInput(sf::Event& event) {
    // Base implementation doesn't handle any input
    return false;
}

void View::setPosition(const sf::Vector2f& position) {
    this->position = position;
}

void View::setSize(const sf::Vector2f& size) {
    this->size = size;
}

void View::setVisible(bool visible) {
    this->visible = visible;
}

sf::Vector2f View::getPosition() const {
    return position;
}

sf::Vector2f View::getSize() const {
    return size;
}

bool View::isVisible() const {
    return visible;
}