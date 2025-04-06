#pragma once

#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include "View.h"

class ViewManager {
public:
    ViewManager(sf::RenderWindow& window);
    ~ViewManager();
    
    void addView(const std::string& name, std::unique_ptr<View> view);
    View* getView(const std::string& name);
    void removeView(const std::string& name);
    
    void update();
    void draw();
    bool handleInput(sf::Event& event);
    
private:
    sf::RenderWindow& window;
    std::unordered_map<std::string, std::unique_ptr<View>> views;
    std::vector<std::string> drawOrder; // Controls the order in which views are drawn
};