#include "WallBreakerView.h"
#include "../Maze.h"

WallBreakerView::WallBreakerView(sf::RenderWindow& window, const sf::Font& font,const Entity& player,std::vector<std::vector<int>>& grid) : View(window) , instructions(font), player(player), grid(grid) {
    highlight.setSize(sf::Vector2f(32, 32)); // Assuming GRID_SIZE is 32
    highlight.setFillColor(sf::Color(0, 255, 0, 100)); // Semi-transparent green

    instructions.setFont(font);
    instructions.setCharacterSize(16);
    instructions.setString("Select a wall to break (arrow keys), or ESC to cancel");
    instructions.setFillColor(sf::Color::Yellow);
    instructions.setPosition(sf::Vector2f(10, window.getSize().y - 100));
    this->visible = false;
}

void WallBreakerView::update() {
    // Update logic if needed
}

void WallBreakerView::draw() {
    float timeLeft = 5.0f - wallBreakerClock.getElapsedTime().asSeconds();
    sf::RectangleShape timerBg;
    timerBg.setSize(sf::Vector2f(200.f, 10.f));
    timerBg.setPosition(sf::Vector2f(GRID_SIZE * GRID_WIDTH / 2 - 100.f, 5.f));
    timerBg.setFillColor(sf::Color(50, 50, 50));
    window.draw(timerBg);
    
    sf::RectangleShape timerFill;
    timerFill.setSize(sf::Vector2f(200.f * (timeLeft / 5.0f), 10.f));
    timerFill.setPosition(sf::Vector2f(GRID_SIZE * GRID_WIDTH / 2 - 100.f, 5.f));
    timerFill.setFillColor(sf::Color(0, 200, 100));
    window.draw(timerFill);
    
    // Highlight valid walls to break
    sf::RectangleShape highlight;
    highlight.setSize(sf::Vector2f(GRID_SIZE, GRID_SIZE));
    highlight.setFillColor(sf::Color(0, 255, 0, 100)); // Semi-transparent green
    
    // Highlight adjacent cells if they are walls
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            // Skip diagonals and center
            if ((dx == 0 && dy == 0) || (dx != 0 && dy != 0)) continue;
            
            int nx = player.pos.x + dx;
            int ny = player.pos.y + dy;
            
            // Check if position is valid and contains a wall
            if (nx >= 0 && nx < GRID_WIDTH && ny >= 0 && ny < GRID_HEIGHT) {
                if (grid[ny][nx] == WALL) {
                    highlight.setPosition(sf::Vector2f(nx * GRID_SIZE, ny * GRID_SIZE));
                    window.draw(highlight);
                }
            }
        }
    }

    window.draw(instructions);
}

void WallBreakerView::setHighlightPosition(const sf::Vector2f& position) {
    highlight.setPosition(position);
}