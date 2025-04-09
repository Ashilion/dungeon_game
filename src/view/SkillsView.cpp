#include "SkillsView.h"
#include <iostream>
#include "SkillTreeView.h"
#include "../Maze.h"

SkillsView::SkillsView(sf::RenderWindow& window, const SkillTree& skillTree)
    : View(window, sf::Vector2f(10.f, GRID_SIZE * GRID_HEIGHT - 150.f), 
           sf::Vector2f(GRID_SIZE * GRID_WIDTH - 20.f, 30.f)),
      skillTree(skillTree), skillsText(font) {
    
    // Initialize the font
    if (!font.openFromFile("assets/arial.ttf")) {
        std::cerr << "Error loading font\n";
    }
    
    // Initialize the skills text
    skillsText.setFont(font);
    skillsText.setCharacterSize(16);
    skillsText.setFillColor(sf::Color::White);
    skillsText.setPosition(sf::Vector2f(15.f, GRID_SIZE * GRID_HEIGHT - 145.f));
    
    // Initialize the background
    background.setSize(size);
    background.setPosition(position);
    background.setFillColor(sf::Color(0, 40, 60, 200));
    
    update();
}

void SkillsView::draw() {
    auto unlockedSkills = skillTree.getUnlockedSkills();
    if (!visible || unlockedSkills.empty()) return;
    
    // Draw the background
    window.draw(background);
    
    // Draw the skills text
    window.draw(skillsText);
}

void SkillsView::update() {
    auto unlockedSkills = skillTree.getUnlockedSkills();
    if (unlockedSkills.empty()) return;
    
    std::string skillList = "Skills: ";
    for (size_t i = 0; i < unlockedSkills.size(); i++) {
        skillList += std::to_string(i+1) + ":" + unlockedSkills[i]->getName() + 
                    " (" + std::to_string(unlockedSkills[i]->getUsesLeft()) + " charges)";
        if (i < unlockedSkills.size() - 1) {
            skillList += ", ";
        }
    }
    
    skillsText.setString(skillList);
}