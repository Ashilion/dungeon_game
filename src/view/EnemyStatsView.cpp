#include "EnemyStatsView.h"
#include <iostream>

EnemyStatsView::EnemyStatsView(sf::RenderWindow& window, 
                              const Entity& player,
                              const std::vector<Entity>& enemies,
                              const std::vector<EnemyType>& enemyTypes)
    : View(window, sf::Vector2f(195.f, 5.f), sf::Vector2f(180.f, 100.f)),
      player(player), enemies(enemies), enemyTypes(enemyTypes), inCombat(false), statsText(font) {
    
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
    background.setFillColor(sf::Color(60, 0, 0, 200));
    
    update();
}

void EnemyStatsView::draw() {
    if (!visible || !inCombat) return;
    
    // Draw the background
    window.draw(background);
    
    // Draw the stats text
    window.draw(statsText);
}

void EnemyStatsView::update() {
    std::string enemyStats = "Enemy\n";
    
    if (inCombat) {
        for (const auto& enemy : enemies) {
            if (std::abs(enemy.pos.x - player.pos.x) + std::abs(enemy.pos.y - player.pos.y) == 1) {
                if (enemy.type >= 0 && enemy.type < static_cast<int>(enemyTypes.size())) {
                    enemyStats += "Type: " + enemyTypes[enemy.type].name +
                                  "\nHP: " + std::to_string(enemy.hp) +
                                  "\nAttack: " + std::to_string(enemy.attack);
                } else {
                    enemyStats += "HP: " + std::to_string(enemy.hp) +
                                  "\nAttack: " + std::to_string(enemy.attack);
                }
                break;
            }
        }
    } else {
        enemyStats += "No enemy nearby";
    }
    
    statsText.setString(enemyStats);
}

void EnemyStatsView::setInCombat(bool inCombat) {
    this->inCombat = inCombat;
    update();
}