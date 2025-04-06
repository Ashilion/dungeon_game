#pragma once

#include "View.h"
#include "../Object.h"
#include <vector>

class InventoryView : public View {
public:
    InventoryView(sf::RenderWindow& window, const std::vector<Object*>& inventory);
    
    void draw() override;
    void update() override;
    // bool handleInput(sf::Event& event) override;
    
    void setShowInventory(bool show);
    void setSelectedItem(int index);
    int getSelectedItem() const;
    
private:
    const std::vector<Object*>& inventory;
    bool showInventory;
    int selectedItem;
    sf::Font font;
    sf::Text inventoryText;
    sf::RectangleShape background;
    sf::Text controlsText;
};