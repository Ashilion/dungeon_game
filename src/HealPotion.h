#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <functional>
#include "Object.h" // Include the base class
// Forward declaration of Entity to avoid circular dependency
struct Entity;

class HealPotion : public Object {
    private:
        int healAmount;
    
    public:
        HealPotion(const sf::Vector2i& pos, int amount, const std::string& texturePath = "assets/heal.png");
        
        // Override use method to heal the entity
        bool use(Entity& entity) override;
        
        // Get the healing amount
        int getHealAmount() const;
        
        // Override description for more detail
        std::string getDescription() const override;
    };