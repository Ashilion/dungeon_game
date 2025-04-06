#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <functional>

// Forward declaration of Entity to avoid circular dependency
struct Entity;

// Base Object class for all items in the game
class Object {
protected:
    sf::Vector2i position;
    std::string name;
    sf::Texture texture;
    sf::Sprite sprite;
    bool collected;
    int value; // General purpose value (potency for potions, damage for weapons)

public:
    Object(const sf::Vector2i& pos, const std::string& objName, int objValue, const std::string& texturePath);
    virtual ~Object() = default;
    
    // Get object position
    sf::Vector2i getPosition() const;
    
    // Set object position
    void setPosition(const sf::Vector2i& pos);
    
    // Get object name
    std::string getName() const;
    
    // Get object value
    int getValue() const;
    
    // Check if object has been collected
    bool isCollected() const;
    
    // Mark object as collected
    void collect();
    
    // Draw the object
    virtual void draw(sf::RenderWindow& window, float alpha = 255.f);
    
    // Virtual use method to be overridden by child classes
    virtual bool use(Entity& entity) = 0;
    
    // Virtual description method for UI tooltips
    virtual std::string getDescription() const;
};