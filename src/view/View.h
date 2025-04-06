#pragma once

#include <SFML/Graphics.hpp>

class View {
public:
    View(sf::RenderWindow& window, sf::Vector2f position = sf::Vector2f(0, 0), 
         sf::Vector2f size = sf::Vector2f(0, 0));
    virtual ~View() = default;
    
    virtual void draw() = 0;
    virtual void update() = 0;
    virtual bool handleInput(sf::Event& event);
    
    void setPosition(const sf::Vector2f& position);
    void setSize(const sf::Vector2f& size);
    void setVisible(bool visible);
    
    sf::Vector2f getPosition() const;
    sf::Vector2f getSize() const;
    bool isVisible() const;
    
protected:
    sf::RenderWindow& window;
    sf::Vector2f position;
    sf::Vector2f size;
    bool visible;
};