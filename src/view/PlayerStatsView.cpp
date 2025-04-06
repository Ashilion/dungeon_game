#include "PlayerStatsView.h"
#include <iostream>

PlayerStatsView::PlayerStatsView(sf::RenderWindow& window, const Entity& player)
    : View(window, sf::Vector2f(5.f, 5.f), sf::Vector2f(180.f, 100.f)),
      player(player), level(1), skillPoints(0), statsText(font) {
    
    // Initialize the font
    if (!font.openFromFile("assets/arial.ttf")) {
        std::cerr << "Error loading font\n";
    }
    
    // Initialize the text
    statsText.setFont(font);
    statsText.setCharacterSize(16);
    statsText.setFillColor(sf::Color::White);
    statsText.setPosition(position);
    
    // Initialize the background
    background.setSize(size);
    background.setPosition(position);
    background.setFillColor(sf::Color(0, 0, 60, 200));
    
    update();
}

void PlayerStatsView::draw() {
    if (!visible) return;
    
    // Draw the background
    window.draw(background);
    
    // Draw the stats text
    window.draw(statsText);
}

void PlayerStatsView::update() {
    // Update the stats text content
    std::string playerStats = "Player\nHP: " + std::to_string(player.hp) +
                       "\nAttack: " + std::to_string(player.attack) +
                       "\nLevel: " + std::to_string(level);

    // Add skill points if any
    if (skillPoints > 0) {
        playerStats += "\nSkill Points: " + std::to_string(skillPoints);
    }
    
    statsText.setString(playerStats);
}

void PlayerStatsView::setLevel(int level) {
    this->level = level;
    update();
}

void PlayerStatsView::setSkillPoints(int points) {
    this->skillPoints = points;
    update();
}