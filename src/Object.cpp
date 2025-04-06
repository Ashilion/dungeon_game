#include "Object.h"
#include <algorithm> // For std::max

// Assuming this is defined somewhere in your project
#ifndef GRID_SIZE
#define GRID_SIZE 32
#endif

// ----------------- Object Implementation -----------------

Object::Object(const sf::Vector2i& pos, const std::string& objName, int objValue, const std::string& texturePath) 
    : position(pos), name(objName), value(objValue), collected(false) , sprite(texture) {
    
    if (!texture.loadFromFile(texturePath)) {
        // Fallback if texture loading fails
        sf::Image img(sf::Vector2u(GRID_SIZE, GRID_SIZE), sf::Color(0, 0, 0));
        texture.loadFromImage(img);
    }
    
    // Configure sprite
    sprite.setTexture(texture);
    
    // Scale sprite to fit grid size
    sf::Vector2u textureSize = texture.getSize();
    float scaleX = static_cast<float>(GRID_SIZE) / textureSize.x;
    float scaleY = static_cast<float>(GRID_SIZE) / textureSize.y;
    sprite.setScale(sf::Vector2f(scaleX, scaleY));
    
    // Set initial position
    sprite.setPosition(sf::Vector2f(position.x * GRID_SIZE, position.y * GRID_SIZE));
}

sf::Vector2i Object::getPosition() const { 
    return position; 
}

void Object::setPosition(const sf::Vector2i& pos) {
    position = pos;
    sprite.setPosition(sf::Vector2f(position.x * GRID_SIZE, position.y * GRID_SIZE));
}

std::string Object::getName() const { 
    return name; 
}

int Object::getValue() const { 
    return value; 
}

bool Object::isCollected() const { 
    return collected; 
}

void Object::collect() { 
    collected = true; 
}

void Object::draw(sf::RenderWindow& window, float alpha) {
    if (!collected) {
        sf::Color color = sprite.getColor();
        color.a = static_cast<uint8_t>(alpha);
        sprite.setColor(color);
        window.draw(sprite);
    }
}

std::string Object::getDescription() const {
    return name;
}