#include "ViewManager.h"

ViewManager::ViewManager(sf::RenderWindow& window) : window(window) {
}

ViewManager::~ViewManager() {
    views.clear();
    drawOrder.clear();
}

void ViewManager::addView(const std::string& name, std::unique_ptr<View> view) {
    views[name] = std::move(view);
    drawOrder.push_back(name);
}

View* ViewManager::getView(const std::string& name) {
    auto it = views.find(name);
    return (it != views.end()) ? it->second.get() : nullptr;
}

void ViewManager::removeView(const std::string& name) {
    // Erase the view from the map
    views.erase(name);

    // Remove from draw order
    for (auto it = drawOrder.begin(); it != drawOrder.end(); ++it) {
        if (*it == name) {
            drawOrder.erase(it);
            break;
        }
    }
}

void ViewManager::update() {
    for (const auto& name : drawOrder) {
        auto& view = views[name];
        if (view && view->isVisible()) {
            view->update();
        }
    }
}

void ViewManager::draw() {
    for (const auto& name : drawOrder) {
        auto& view = views[name];
        if (view && view->isVisible()) {
            view->draw();
        }
    }
}

bool ViewManager::handleInput(sf::Event& event) {
    // Process views in reverse order (top to bottom)
    for (auto it = drawOrder.rbegin(); it != drawOrder.rend(); ++it) {
        auto& view = views[*it];
        if (view && view->isVisible()) {
            if (view->handleInput(event)) {
                return true; // Input was handled
            }
        }
    }
    return false; // No view handled the input
}