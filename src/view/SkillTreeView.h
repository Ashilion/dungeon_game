#ifndef SKILL_H
#define SKILL_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <functional>
#include <memory>
#include <random>
#include <iostream>
#include "View.h"

// Forward declaration
class Game;

// Skill class to represent individual skills
class Skill {
public:
    enum class SkillType {
        WALL_BREAKER,    // Break one wall
        VISION_BOOST,    // Increase vision radius
        HEALTH_BOOST,    // Permanent health increase
        ATTACK_BOOST,    // Permanent attack increase
        // ENEMY_JUMP,      // Jump over an enemy
        // DOUBLE_JUMP,     // Jump over two consecutive obstacles
    };

private:
    SkillType type;
    std::string name;
    std::string description;
    int usesLeft;        // How many times skill can be used (0 for permanent effects)
    bool unlocked;
    sf::Texture iconTexture;
    sf::Sprite iconSprite;

public:
    Skill(SkillType type, const std::string& name, const std::string& description, int uses, const std::string& iconPath);

    SkillType getType() const { return type; }
    std::string getName() const { return name; }
    std::string getDescription() const { return description; }
    int getUsesLeft() const { return usesLeft; }
    bool isUnlocked() const { return unlocked; }
    sf::Sprite& getSprite() { return iconSprite; }

    void unlock() { unlocked = true; }
    bool use() {
        if (!unlocked || usesLeft <= 0) return false;
        if (usesLeft > 0) usesLeft--;
        return true;
    }
    
    void resetUses(int uses) { usesLeft = uses; }
};

// SkillTree class to manage skills and skill selection
class SkillTree : public View {
private:
    std::vector<std::unique_ptr<Skill>> skills;
    sf::Font font;
    sf::Text titleText;
    sf::Text descriptionText;
    sf::Text instructionText;
    sf::RectangleShape background;
    sf::RectangleShape selectionHighlight;
    int selectedSkillIndex;
    bool isActive;
    std::function<void(Skill*)> onSkillSelected;

public:
    SkillTree(sf::RenderWindow& window);
    
    void initializeSkills();
    void setPosition(sf::Vector2f position, sf::Vector2f size);
    void repositionSkills();
    void setCallback(std::function<void(Skill*)> callback);
    void activate();
    void deactivate();
    bool isActiveState() const;
    void selectRandomSkills(int count);
    void handleInput(const sf::Clock& inputClock, float inputDelay);
    void draw() override; // Override from View
    void update() override; // Override from View
    std::vector<Skill*> getUnlockedSkills() const;
};

#endif // SKILL_H