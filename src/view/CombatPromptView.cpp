#include "CombatPromptView.h"
#include <iostream>

// Assuming GRID_SIZE, GRID_WIDTH are defined globally
const int GRID_SIZE = 32;
const int GRID_WIDTH = 20;

CombatPromptView::CombatPromptView(sf::RenderWindow& window)
    : View(window, sf::Vector2f(GRID_SIZE * GRID_WIDTH - 220.f, 5.f), sf::Vector2f(200.f, 30.f)),
      showPrompt(false) , promptText(font) {
    
    // Initialize the font
    if (!font.openFromFile("assets/arial.ttf")) {
        std::cerr << "Error loading font\n";
    }
    
    // Initialize the prompt text
    promptText.setFont(font);
    promptText.setCharacterSize(16);
    promptText.setFillColor(sf::Color::White);
    promptText.setString("Press SPACE to attack");
    promptText.setPosition(sf::Vector2f(GRID_SIZE * GRID_WIDTH - 210.f, 10.f));
    
    // Initialize the background
    background.setSize(size);
    background.setPosition(position);
    background.setFillColor(sf::Color(80, 0, 0, 200));
}

void CombatPromptView::draw() {
    if (!visible || !showPrompt) return;
    
    // Draw the background
    window.draw(background);
    
    // Draw the prompt text
    window.draw(promptText);
}

void CombatPromptView::update() {
    // Nothing to update dynamically
}

void CombatPromptView::setShowPrompt(bool show) {
    showPrompt = show;
}