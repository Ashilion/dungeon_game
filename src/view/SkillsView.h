#pragma once

#include "View.h"
#include "SkillTreeView.h"
#include <vector>

class SkillsView : public View {
public:
    SkillsView(sf::RenderWindow& window, const SkillTree& skillTree);
    
    void draw() override;
    void update() override;
    
private:
    const SkillTree& skillTree;
    sf::Font font;
    sf::Text skillsText;
    sf::RectangleShape background;
};