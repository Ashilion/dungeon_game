#pragma once

#include "View.h"

class CombatPromptView : public View {
public:
    CombatPromptView(sf::RenderWindow& window);
    
    void draw() override;
    void update() override;
    
    void setShowPrompt(bool show);
    
private:
    bool showPrompt;
    sf::Font font;
    sf::Text promptText;
    sf::RectangleShape background;
};