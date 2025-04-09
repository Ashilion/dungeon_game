#include "InventoryView.h"
#include "../HealPotion.h"
#include <iostream>
#include "../Maze.h"

InventoryView::InventoryView(sf::RenderWindow& window, const std::vector<Object*>& inventory)
    : View(window, sf::Vector2f(10.f, GRID_SIZE * GRID_HEIGHT - 220.f), 
           sf::Vector2f(GRID_SIZE * GRID_WIDTH - 20.f, 200.f)),
      inventory(inventory), showInventory(false), selectedItem(0) , inventoryText(font), controlsText(font){
    
    // Initialiser la font
    if (!font.openFromFile("assets/arial.ttf")) {
        std::cerr << "Error loading font\n";
    }
    
    // Initialisation des éléments graphiques
    inventoryText.setFont(font);
    inventoryText.setCharacterSize(16);
    inventoryText.setFillColor(sf::Color::White);
    inventoryText.setPosition(position);
    
    background.setSize(size);
    background.setPosition(position);
    background.setFillColor(sf::Color(20, 20, 40, 230));
    background.setOutlineColor(sf::Color(100, 100, 120));
    background.setOutlineThickness(2.f);

    controlsText.setFont(font);
    controlsText.setCharacterSize(14);
    controlsText.setFillColor(sf::Color(150, 150, 150));
    controlsText.setString("Up/Down: Navigate | U: Use Item | ESC: Close");
    controlsText.setPosition(sf::Vector2f(GRID_SIZE * GRID_WIDTH / 2 - 140.f, 
                                        GRID_SIZE * GRID_HEIGHT - 40.f));

    
    this->visible = false;
    
    update();
}

void InventoryView::draw() {
    if (!this->visible) return;
    window.draw(background);
    window.draw(inventoryText);
    
    // Affiche le texte d'inventaire
    if (inventory.empty()) {
        sf::Text emptyText(font);
        emptyText.setFont(font);
        emptyText.setCharacterSize(16);
        emptyText.setFillColor(sf::Color(150, 150, 150));
        emptyText.setString("Your inventory is empty");
        emptyText.setPosition(sf::Vector2f(GRID_SIZE * GRID_WIDTH / 2 - 80.f, 
                                          GRID_SIZE * GRID_HEIGHT - 180.f));
        window.draw(emptyText);
    } else {
        // Dessine la position du texte d'inventaire
        window.draw(controlsText);
    }
}

void InventoryView::update() {
    std::string invText = "Inventory (" + std::to_string(inventory.size()) + " items) [I to toggle]\n";
    
    if (this->visible && !inventory.empty()) {
        for (size_t i = 0; i < inventory.size(); i++) {
            if (i == static_cast<size_t>(selectedItem)) {
                invText += "> ";
            } else {
                invText += "  ";
            }
            
            invText += std::to_string(i+1) + ": " + inventory[i]->getDescription();
            
            // Check if it's a potion
            HealPotion* potion = dynamic_cast<HealPotion*>(inventory[i]);
            if (potion) {
                invText += " [press U to use]";
            }
            
            invText += "\n";
        }
    }
    
    inventoryText.setString(invText);
}

// bool InventoryView::handleInput(sf::Event& event) {
//     if (!visible || !showInventory || inventory.empty()) return false;

//     if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up) || 
//                 sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) {
//                 if (inputClock.getElapsedTime().asSeconds() >= MOVE_DELAY) {
//                     selectedInventoryItem = (selectedInventoryItem + inventory.size() - 1) % inventory.size();
//                     updateInventoryDisplay();
//                     inputClock.restart();
//                     return;
//                 }
//             }
//             else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down) || 
//                      sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
//                 if (inputClock.getElapsedTime().asSeconds() >= MOVE_DELAY) {
//                     selectedInventoryItem = (selectedInventoryItem + 1) % inventory.size();
//                     updateInventoryDisplay();
//                     inputClock.restart();
//                     return;
//                 }
//             }
//             else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::U)) {
//                 if (inputClock.getElapsedTime().asSeconds() >= MOVE_DELAY) {
//                     useSelectedItem();
//                     inputClock.restart();
//                     return;
//                 }
//             }
            
//             // Allow closing inventory with Escape
//             if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) {
//                 if (inputClock.getElapsedTime().asSeconds() >= MOVE_DELAY) {
//                     showInventory = false;
//                     updateInventoryDisplay();
//                     inputClock.restart();
//                     return;
//                 }
//             }
            
//             // Block other inputs when inventory is open
//             return;

//     return false;
// }

void InventoryView::setShowInventory(bool show) {
    this->visible = show;
    update();
}

void InventoryView::setSelectedItem(int index) {
    if (!inventory.empty()) {
        selectedItem = index % inventory.size();
        update();
    }
}

int InventoryView::getSelectedItem() const {
    return selectedItem;
}