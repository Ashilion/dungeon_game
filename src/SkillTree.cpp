#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <functional>
#include <memory>
#include "SkillTree.h"

// Skill class to represent individual skills



Skill::Skill(SkillType type, const std::string& name, const std::string& description, int uses, const std::string& iconPath) 
    : type(type), name(name), description(description), usesLeft(uses), unlocked(false), iconSprite(iconTexture) {
    
    if (!iconTexture.loadFromFile(iconPath)) {
        std::cerr << "Error loading skill icon: " << iconPath << std::endl;
    }
    iconSprite = sf::Sprite(iconTexture);
    
    // Resize icon to fit skill tree display
    sf::Vector2u textureSize = iconTexture.getSize();
    float scale = 48.0f / std::max(textureSize.x, textureSize.y);
    iconSprite.setScale(sf::Vector2f( scale, scale));
}



SkillTree::SkillTree() : selectedSkillIndex(0), isActive(false), titleText(font), descriptionText(font), instructionText(font) {
        if (!font.openFromFile("assets/arial.ttf")) {
            std::cerr << "Error loading font\n";
        }
        
        // Initialize text elements
        titleText.setFont(font);
        titleText.setCharacterSize(24);
        titleText.setFillColor(sf::Color::White);
        titleText.setString("Select a New Skill");
        
        descriptionText.setFont(font);
        descriptionText.setCharacterSize(16);
        descriptionText.setFillColor(sf::Color::White);
        
        instructionText.setFont(font);
        instructionText.setCharacterSize(16);
        instructionText.setFillColor(sf::Color::Yellow);
        instructionText.setString("Arrow keys to navigate, ENTER to select");
        
        // Semi-transparent background
        background.setFillColor(sf::Color(0, 0, 0, 200));
        
        // Selection highlight
        selectionHighlight.setFillColor(sf::Color(100, 100, 255, 100));
        selectionHighlight.setOutlineColor(sf::Color::White);
        selectionHighlight.setOutlineThickness(2);
        selectionHighlight.setSize(sf::Vector2f(180, 60));
        
        // Initialize skills
        initializeSkills();
    }
    
    void SkillTree::initializeSkills() {
        // Create all possible skills
        skills.push_back(std::make_unique<Skill>(
            Skill::SkillType::WALL_BREAKER, 
            "Wall Breaker", 
            "Break one wall of your choice", 
            1, 
            "assets/axe.jpeg"
        ));
        
        skills.push_back(std::make_unique<Skill>(
            Skill::SkillType::VISION_BOOST, 
            "Enhanced Vision", 
            "Increase vision radius by 3", 
            0, 
            "assets/vision.png"
        ));
        skills.push_back(std::make_unique<Skill>(
            Skill::SkillType::HEALTH_BOOST, 
            "Health Boost", 
            "Increase max health by 25", 
            0, 
            "assets/health.jpg"
        ));
        
        skills.push_back(std::make_unique<Skill>(
            Skill::SkillType::ATTACK_BOOST, 
            "Attack Boost", 
            "Increase attack power by 10", 
            0, 
            "assets/sword.jpeg"
        ));
        
        // skills.push_back(std::make_unique<Skill>(
        //     Skill::SkillType::ENEMY_JUMP, 
        //     "Enemy Jump", 
        //     "Jump over one enemy without combat", 
        //     1, 
        //     "src/skill_enemy_jump.png"
        // ));
        
        // skills.push_back(std::make_unique<Skill>(
        //     Skill::SkillType::DOUBLE_JUMP, 
        //     "Double Jump", 
        //     "Jump over two consecutive obstacles", 
        //     1, 
        //     "src/skill_double_jump.png"
        // ));
        
        
        
        // skills.push_back(std::make_unique<Skill>(
        //     Skill::SkillType::TELEPORT, 
        //     "Teleport", 
        //     "Teleport to a visible empty cell once", 
        //     1, 
        //     "src/skill_teleport.png"
        // ));
    }
    
    void SkillTree::setPosition(sf::Vector2f position, sf::Vector2f size) {
        background.setSize(size);
        background.setPosition(position);
        
        titleText.setPosition(sf::Vector2f(position.x + 20, position.y + 20));
        instructionText.setPosition(sf::Vector2f(position.x + 20, position.y + size.y - 40));
        descriptionText.setPosition(sf::Vector2f(position.x + 20, position.y + size.y - 80));
        
        // Position skill icons in a grid layout
        repositionSkills();
    }
    
    void SkillTree::repositionSkills() {
        float startX = background.getPosition().x + 40;
        float startY = background.getPosition().y + 80;
        int skillsPerRow = 3;
        float skillSpacing = 200.0f;
        
        for (size_t i = 0; i < skills.size(); i++) {
            int row = i / skillsPerRow;
            int col = i % skillsPerRow;
            
            skills[i]->getSprite().setPosition(sf::Vector2f(
                startX + col * skillSpacing,
                startY + row * skillSpacing)
            );
        }
    }
    
    void SkillTree::setCallback(std::function<void(Skill*)> callback) {
        onSkillSelected = callback;
    }
    
    void SkillTree::activate() {
        isActive = true;
        selectedSkillIndex = 0;
        updateDescription();
    }
    
    void SkillTree::deactivate() {
        isActive = false;
    }
    
    bool SkillTree::isActiveState() const {
        return isActive;
    }
    
    void SkillTree::selectRandomSkills(int count) {
        // Reset all skills to be not available for selection
        for (auto& skill : skills) {
            skill->resetUses(0);
        }
        
        // Choose random skills to offer
        std::vector<int> availableIndices;
        for (size_t i = 0; i < skills.size(); i++) {
            availableIndices.push_back(i);
        }
        
        std::random_device rd;
        std::mt19937 rng(rd());
        std::shuffle(availableIndices.begin(), availableIndices.end(), rng);
        
        // Take the first 'count' skills and make them available
        for (int i = 0; i < static_cast<int>(availableIndices.size()); i++) {
            int index = availableIndices[i];
            auto& skill = skills[index];
            
            // Set appropriate uses based on skill type
            if (skill->getType() == Skill::SkillType::WALL_BREAKER ){
                skill->resetUses(1);
            } else {
                // Permanent skills
                skill->resetUses(10);
            }
        }
    }
    
    void SkillTree::handleInput(const sf::Clock& inputClock, float inputDelay) {
        if (inputClock.getElapsedTime().asSeconds() < inputDelay) {
            return;
        }
        
        bool inputHandled = false;
        int prevIndex = selectedSkillIndex;
        
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
            if (selectedSkillIndex % 3 > 0) {
                selectedSkillIndex--;
                inputHandled = true;
            }
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
            if (selectedSkillIndex % 3 < 2 && selectedSkillIndex < static_cast<int>(skills.size()) - 1) {
                selectedSkillIndex++;
                inputHandled = true;
            }
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) {
            if (selectedSkillIndex >= 3) {
                selectedSkillIndex -= 3;
                inputHandled = true;
            }
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) {
            if (selectedSkillIndex + 3 < static_cast<int>(skills.size())) {
                selectedSkillIndex += 3;
                inputHandled = true;
            }
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Enter)) {
            if (skills[selectedSkillIndex]->getUsesLeft() > 0) {
                if (onSkillSelected) {
                    onSkillSelected(skills[selectedSkillIndex].get());
                }
                deactivate();
                inputHandled = true;
            }
        }
        
        if (inputHandled && prevIndex != selectedSkillIndex) {
            updateDescription();
        }
    }
    
    void SkillTree::updateDescription() {
        if (selectedSkillIndex >= 0 && selectedSkillIndex < static_cast<int>(skills.size())) {
            auto& skill = skills[selectedSkillIndex];
            
            std::string desc = skill->getName() + "\n" + skill->getDescription();
            if (skill->getUsesLeft() > 0) {
                desc += "\nUses: " + std::to_string(skill->getUsesLeft());
            } else if (skill->getUsesLeft() == 0 && skill->isUnlocked()) {
                desc += "\n(Already unlocked)";
            } else {
                desc += "\n(Not available)";
            }
            
            descriptionText.setString(desc);
        }
    }
    
    void SkillTree::draw(sf::RenderWindow& window) {
        // if (!isActive) return;
        
        window.draw(background);
        window.draw(titleText);
        window.draw(descriptionText);
        window.draw(instructionText);
        
        // Draw selection highlight behind the selected skill
        if (selectedSkillIndex >= 0 && selectedSkillIndex < static_cast<int>(skills.size())) {
            selectionHighlight.setPosition(sf::Vector2f(
                skills[selectedSkillIndex]->getSprite().getPosition().x - 10,
                skills[selectedSkillIndex]->getSprite().getPosition().y - 10)
            );
            window.draw(selectionHighlight);
        }
        
        // Draw available skills
        for (size_t i = 0; i < skills.size(); i++) {
            if (skills[i]->getUsesLeft() > 0 || skills[i]->isUnlocked()) {
                // Draw available skills at full opacity
                skills[i]->getSprite().setColor(sf::Color(255, 255, 255, 255));
            } else {
                // Draw unavailable skills with reduced opacity
                skills[i]->getSprite().setColor(sf::Color(100, 100, 100, 128));
            }
            window.draw(skills[i]->getSprite());
        }
    }
    
    std::vector<Skill*> SkillTree::getUnlockedSkills() {
        std::vector<Skill*> result;
        for (auto& skill : skills) {
            if (skill->isUnlocked()) {
                result.push_back(skill.get());
            }
        }
        return result;
    }
