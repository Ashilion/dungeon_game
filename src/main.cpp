#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <random>
#include <stack>
#include <chrono>
#include "Maze.h"
#include "view/SkillTreeView.h" 
#include "Object.h"
#include "HealPotion.h"
#include "Entity.h"
#include "view/ViewManager.h"
#include "view/GameGridView.h"
#include "view/PlayerStatsView.h"
#include "view/EnemyStatsView.h"
#include "view/SkillsView.h"
#include "view/InventoryView.h"
#include "view/WallBreakerView.h"
#include "view/WinView.h"
#include "view/GameOverView.h"
#include "Weapon.h"
#include "Armor.h"
#include "Key.h"

#define ENEMY_COUNT 5
#define ITEM_COUNT 3
#define MOVE_DELAY 0.1f
#define ATTACK_DELAY 0.5f
#define PLAYER_BASE_HP 100
#define PLAYER_BASE_ATTACK 15
#define MAX_LEVEL 5 // Nombre de niveau / Niveau ou le coffre se trouve

int VISION_RADIUS = 8;  // Rayon de vision en nombre de cases  

class Game {
private:
    std::vector<std::vector<int>> grid;
    Entity player;
    sf::Clock moveClock;
    sf::Clock attackClock;
    int level;
    std::mt19937 rng;
    std::vector<Entity> enemies;
    bool inCombat;
    bool showAttackPrompt;
    sf::Font font;
    int skillPoints;
    bool showSkillTree;
    sf::Clock inputClock;
    bool wallBreakerMode = false;
    sf::Clock wallBreakerClock;
    SkillTree* skillTree;
    // New inventory system
    std::vector<Object*> worldObjects;      // Objects in the world
    std::vector<Object*> inventory;         // Player's inventory
    bool showInventory = false;             // Toggle for inventory display
    int selectedInventoryItem = 0;          // Currently selected inventory item

    // View manager to handle all views
    std::unique_ptr<ViewManager> viewManager;

    // Define enemy types
    std::vector<EnemyType> enemyTypes = {
        {30, 10, sf::Color(200, 50, 50), "Goblin"},      // Weak enemy
        {50, 15, sf::Color(200, 100, 50), "Orc"},        // Medium enemy
        {80, 20, sf::Color(200, 50, 100), "Troll"}      // Strong enemy
    };

    sf::Sprite createSprite(const sf::Texture& texture) {
        sf::Sprite sprite(texture);
        
        // Get the texture size
        sf::Vector2u textureSize = texture.getSize();
        
        // Calculate scale to make sprite exactly GRID_SIZE pixels in both dimensions
        float scaleX = static_cast<float>(GRID_SIZE) / textureSize.x;
        float scaleY = static_cast<float>(GRID_SIZE) / textureSize.y;
        
        // Set the calculated scale
        sprite.setScale(sf::Vector2f(scaleX, scaleY));
        
        return sprite;
    }

public:
    Game(sf::RenderWindow& window) : player(sf::Vector2i(0, 0), PLAYER_BASE_HP, PLAYER_BASE_ATTACK), 
             level(1), inCombat(false), showAttackPrompt(false), skillPoints(0), showSkillTree(false), 
             wallBreakerMode(false), viewManager(std::make_unique<ViewManager>(window)) {
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        rng.seed(seed);
        
        if (!font.openFromFile("assets/arial.ttf")) {
            std::cerr << "Error loading font\n";
        }
        viewManager->addView("gameWorld", std::make_unique<GameGridView>(window, grid, player, enemies));
        viewManager->addView("playerStats", std::make_unique<PlayerStatsView>(window, player));
        viewManager->addView("enemyStats", std::make_unique<EnemyStatsView>(window,player, enemies, enemyTypes));
        viewManager->addView("inventory", std::make_unique<InventoryView>(window, inventory));
        viewManager->addView("wallBreaker", std::make_unique<WallBreakerView>(window, font, player,grid));
        viewManager->addView("win", std::make_unique<WinView>(window, font));
        viewManager->addView("gameOver", std::make_unique<GameOverView>(window, font));
        viewManager->addView("skillTree", std::make_unique<SkillTree>(window));
        skillTree = dynamic_cast<SkillTree*>(viewManager->getView("skillTree"));
        viewManager->addView("skills", std::make_unique<SkillsView>(window, *skillTree));
        skillTree->setPosition(sf::Vector2f(GRID_SIZE * 2, GRID_SIZE * 2), 
                         sf::Vector2f(GRID_SIZE * (GRID_WIDTH - 4), GRID_SIZE * (GRID_HEIGHT - 4)));
    
        // Set callback for when a skill is selected
        skillTree->setCallback([this](Skill* skill) {
            applySkill(skill);
            skillPoints--;
            updateStatsDisplay();
            showSkillTree = false;
        });
        
        // Initialize the view manager and add views

        generateNewLevel();
    }

    // Clean up dynamically allocated objects
    ~Game() {
        for (auto* obj : worldObjects) {
            delete obj;
        }
        for (auto* obj : inventory) {
            delete obj;
        }
    }

    void applySkill(Skill* skill) {
        skill->unlock();
        GameGridView* gameGridViewTemp ;
        switch (skill->getType()) {
            case Skill::SkillType::HEALTH_BOOST:
                player.hp += 25;
                break;
            case Skill::SkillType::ATTACK_BOOST:
                player.attack += 10;
                break;
            case Skill::SkillType::VISION_BOOST:
                VISION_RADIUS +=3;
                gameGridViewTemp = dynamic_cast<GameGridView*>(viewManager->getView("gameWorld"));
                gameGridViewTemp->setVisionRadius(VISION_RADIUS);
                break;
            // Other skill types will be used when the player activates them
            default:
                break;
        }
    }
    
    void generateNewLevel() {
        // Clear previous objects
        for (auto* obj : worldObjects) {
            if (obj != nullptr) {
                delete obj;
            }
        }
        worldObjects.clear();
    
        // Clear enemies
        enemies.clear();
    
        // Generate a new maze
        Maze maze(rng());
        
        if (level < MAX_LEVEL) {
            maze.placeStairs(); 
        } else {
            maze.placeTreasure();
        }
        grid = maze.getGrid();
        // Ensure the grid is valid
        if (grid.empty() || grid.size() != GRID_HEIGHT || grid[0].size() != GRID_WIDTH) {
            std::cerr << "Error: Invalid grid dimensions!" << std::endl;
            return;
        }
    
        // Place player
        do {
            player.pos.x = std::uniform_int_distribution<int>(1, GRID_WIDTH - 2)(rng);
            player.pos.y = std::uniform_int_distribution<int>(1, GRID_HEIGHT - 2)(rng);
        } while (grid[player.pos.y][player.pos.x] != EMPTY);
        grid[player.pos.y][player.pos.x] = PLAYER;
    
        // Place enemies
        for (int i = 0; i < ENEMY_COUNT; i++) {
            sf::Vector2i enemyPos;
            int enemyType = std::min(level / 2, 2); // Progressively stronger enemies
            float levelMultiplier = 1.0f + (level - 1) * 0.2f; // 20% stronger per level
    
            do {
                enemyPos.x = std::uniform_int_distribution<int>(1, GRID_WIDTH - 2)(rng);
                enemyPos.y = std::uniform_int_distribution<int>(1, GRID_HEIGHT - 2)(rng);
            } while (grid[enemyPos.y][enemyPos.x] != EMPTY || 
                     std::abs(enemyPos.x - player.pos.x) < 5 || 
                     std::abs(enemyPos.y - player.pos.y) < 5);
    
            int enemyHp = enemyTypes[enemyType].baseHp * levelMultiplier;
            int enemyAttack = enemyTypes[enemyType].baseAttack * levelMultiplier;
            grid[enemyPos.y][enemyPos.x] = ENEMY;
            enemies.push_back(Entity(enemyPos, enemyHp, enemyAttack, enemyType));
        }
    
        // Place heal potions
        HealPotion::generateHealPotions(grid, worldObjects, ITEM_COUNT, level, rng);

        // Place weapons
        Weapon::generateWeapons(grid, worldObjects, ITEM_COUNT, level, rng);

        // Place armors
        Armor::generateArmors(grid, worldObjects, ITEM_COUNT, level, rng);

        // Place the key
        if (level == MAX_LEVEL) {
            Key::generateKey(grid, worldObjects, rng);
        }
    
        // Increment skill points and offer new skills
        if (level > 1) {
            skillPoints++;
            skillTree->selectRandomSkills(3); // Offer 3 skills to choose from
            showSkillTree = true;
            viewManager->getView("skillTree")->setVisible(showSkillTree);
        }
    
        updateStatsDisplay();
        updateInventoryDisplay();
    }
    
    bool isAdjacentToEnemy(sf::Vector2i pos) {
        for (const auto& enemy : enemies) {
            if (std::abs(enemy.pos.x - pos.x) + std::abs(enemy.pos.y - pos.y) == 1) {
                return true;
            }
        }
        return false;
    }
    
    void updateStatsDisplay() {
        // Delegate stats update to PlayerStatsView and EnemyStatsView
        viewManager->getView("playerStats")->setVisible(true);
        viewManager->getView("playerStats")->update();
        viewManager->getView("enemyStats")->setVisible(inCombat);
        viewManager->getView("enemyStats")->update();
    }

    void updateInventoryDisplay() {
        // Delegate inventory update to InventoryView
        viewManager->getView("inventory")->setVisible(showInventory);
        viewManager->getView("inventory")->update();
    }

    void updateWallBreakerDisplay() {
        // Delegate wall breaker update to WallBreakerView
        viewManager->getView("wallBreaker")->setVisible(wallBreakerMode);
        viewManager->getView("wallBreaker")->update();
    }

    void updateWinDisplay() {
        // Delegate win update to WinView
        viewManager->getView("win")->setVisible(level > MAX_LEVEL);
        // viewManager->getView("win")->update();
    }

    void updateGameOverDisplay() {
        // Delegate game over update to GameOverView
        viewManager->getView("gameOver")->setVisible(isGameOver());
        viewManager->getView("gameOver")->update();
    }
    
    void collectItem(const sf::Vector2i& pos) {
        for (auto* obj : worldObjects) {
            if (!obj->isCollected() && obj->getPosition() == pos) {
                std::cout << "Collecting " << obj->getName() << std::endl;
                // Collect the object
                //remove the object from the world objects vector
                auto it = std::remove(worldObjects.begin(), worldObjects.end(), obj);
                worldObjects.erase(it, worldObjects.end());

                obj->collect();
                inventory.push_back(obj);
                
                // Update text display
                std::cout << "Collected " << obj->getName() << std::endl;
                updateInventoryDisplay();
                return;
            }
        }
    }
    
    void useSelectedItem() {
        InventoryView* inventoryView = dynamic_cast<InventoryView*>(viewManager->getView("inventory"));
        selectedInventoryItem= inventoryView->getSelectedItem();
        if (inventory.empty() || selectedInventoryItem >= inventory.size()) return;
        // Check if it's a heal potion
        HealPotion* potion = dynamic_cast<HealPotion*>(inventory[selectedInventoryItem]);
        if (potion) {
            // Use the potion on the player
            if (potion->use(player)) {
                std::cout << "Used " << potion->getName() << " and healed for " 
                          << potion->getHealAmount() << " HP" << std::endl;
                
                // Remove the potion from inventory
                delete inventory[selectedInventoryItem];
                inventory.erase(inventory.begin() + selectedInventoryItem);
                
                // Adjust selected item if needed
                if (!inventory.empty()) {
                    selectedInventoryItem = std::min(selectedInventoryItem, 
                                                  (int)inventory.size() - 1);
                }
                
                updateStatsDisplay();
                updateInventoryDisplay();
            }
        } else {
            // Handle other item types
            Object* item = inventory[selectedInventoryItem];
            
            // Apply effects based on item name
            if (item->getName() == "Weapon") {
                player.attack += item->getValue();
                std::cout << "Used " << item->getName() << " and gained " 
                          << item->getValue() << " attack" << std::endl;
                
                // Remove from inventory
                delete inventory[selectedInventoryItem];
                inventory.erase(inventory.begin() + selectedInventoryItem);
                
                // Adjust selected item
                if (!inventory.empty()) {
                    selectedInventoryItem = std::min(selectedInventoryItem, 
                                                  (int)inventory.size() - 1);
                }
                
                updateStatsDisplay();
                updateInventoryDisplay();
            }
            else if (item->getName() == "Armor") {
                player.hp += item->getValue();
                std::cout << "Used " << item->getName() << " and gained " 
                          << item->getValue() << " HP" << std::endl;
                
                // Remove from inventory
                delete inventory[selectedInventoryItem];
                inventory.erase(inventory.begin() + selectedInventoryItem);
                
                // Adjust selected item
                if (!inventory.empty()) {
                    selectedInventoryItem = std::min(selectedInventoryItem, 
                                                  (int)inventory.size() - 1);
                }
                
                updateStatsDisplay();
                updateInventoryDisplay();
            }
        }
    }
    
    void handleInput() {
        if (moveClock.getElapsedTime().asSeconds() < MOVE_DELAY) return;
        if (player.hp <= 0) return;

        // Handle inventory toggling
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::I)) {
            if (inputClock.getElapsedTime().asSeconds() >= MOVE_DELAY) {
                showInventory = !showInventory;
                viewManager->getView("inventory")->setVisible(showInventory);
                updateInventoryDisplay();
                inputClock.restart();
                return;
            }
        }

        // Handle inventory navigation and usage when inventory is shown
        if (showInventory && !inventory.empty()) {
            // Get the InventoryView instance
            InventoryView* inventoryView = dynamic_cast<InventoryView*>(viewManager->getView("inventory"));
            if (!inventoryView) {
                std::cerr << "Error: InventoryView not found!" << std::endl;
                return;
            }
        
            // Handle moving up in the inventory
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up) || 
                sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) {
                if (inputClock.getElapsedTime().asSeconds() >= MOVE_DELAY) {
                    int currentItem = inventoryView->getSelectedItem();
                    int newItem = (currentItem + inventory.size() - 1) % inventory.size();
                    inventoryView->setSelectedItem(newItem);
                    updateInventoryDisplay();
                    inputClock.restart();
                    return;
                }
            }
            // Handle moving down in the inventory
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down) || 
                     sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
                if (inputClock.getElapsedTime().asSeconds() >= MOVE_DELAY) {
                    int currentItem = inventoryView->getSelectedItem();
                    int newItem = (currentItem + 1) % inventory.size();
                    inventoryView->setSelectedItem(newItem);
                    updateInventoryDisplay();
                    inputClock.restart();
                    return;
                }
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::U)) {
                if (inputClock.getElapsedTime().asSeconds() >= MOVE_DELAY) {
                    useSelectedItem();
                    inputClock.restart();
                    return;
                }
            }

            // Allow closing inventory with Escape
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) {
                if (inputClock.getElapsedTime().asSeconds() >= MOVE_DELAY) {
                    showInventory = false;
                    updateInventoryDisplay();
                    inputClock.restart();
                    return;
                }
            }

            // Block other inputs when inventory is open
            return;
        }

        // Handle skill tree if it's active
        if (showSkillTree) {
            skillTree->handleInput(inputClock, MOVE_DELAY);
            if (inputClock.getElapsedTime().asSeconds() >= MOVE_DELAY) {
                inputClock.restart();
            }
            return;
        }

        if (wallBreakerMode) {
            // Time limit for selecting a wall (5 seconds)
            if (wallBreakerClock.getElapsedTime().asSeconds() > 5.0f) {
                viewManager->getView("wallBreaker")->setVisible(false);
                wallBreakerMode = false;
                return;
            }

            sf::Vector2i targetPos = player.pos;
            bool selected = false;

            // Allow selecting adjacent walls
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up) || 
                sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) {
                targetPos.y = std::max(0, player.pos.y - 1);
                selected = true;
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down) || 
                     sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
                targetPos.y = std::min(GRID_HEIGHT - 1, player.pos.y + 1);
                selected = true;
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) || 
                     sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
                targetPos.x = std::max(0, player.pos.x - 1);
                selected = true;
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) || 
                     sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
                targetPos.x = std::min(GRID_WIDTH - 1, player.pos.x + 1);
                selected = true;
            }

            // Cancel with Escape
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) {
                updateWallBreakerDisplay();
                viewManager->getView("wallBreaker")->setVisible(false);
                wallBreakerMode = false;
                return;
            }

            if (selected && grid[targetPos.y][targetPos.x] == WALL) {
                // Break the wall!
                grid[targetPos.y][targetPos.x] = EMPTY;
                updateWallBreakerDisplay();
                viewManager->getView("wallBreaker")->setVisible(false);
                wallBreakerMode = false;

                // Consume a use from the Wall Breaker skill
                for (auto& skill : skillTree->getUnlockedSkills()) {
                    if (skill->getType() == Skill::SkillType::WALL_BREAKER) {
                        skill->use();
                        break;
                    }
                }

                moveClock.restart();
            }

            return;
        }

        // Toggle skill tree with 'T' key
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::T) && skillPoints > 0) {
            if (inputClock.getElapsedTime().asSeconds() >= MOVE_DELAY) {
                showSkillTree = !showSkillTree;
                viewManager->getView("skills")->setVisible(showSkillTree);
                inputClock.restart();
                return;
            }
        }

        // Use active skills with number keys
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num1) || 
            sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Numpad1)) {
            useActiveSkill(0);
            inputClock.restart();
            return;
        }

        sf::Vector2i newPos = player.pos;
        bool moved = false;

        if (showAttackPrompt && attackClock.getElapsedTime().asSeconds() >= ATTACK_DELAY) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)) {
                for (auto& enemy : enemies) {
                    if (std::abs(enemy.pos.x - player.pos.x) + std::abs(enemy.pos.y - player.pos.y) == 1) {
                        enemy.hp -= player.attack;
                        if (enemy.hp <= 0) {
                            grid[enemy.pos.y][enemy.pos.x] = EMPTY;
                        }
                        if (enemy.hp > 0) {
                            player.hp -= enemy.attack;
                        }
                        attackClock.restart();
                        break;
                    }
                }
                std::vector<Entity> newEnemies;

                for (const auto& e : enemies) {
                    if (e.hp > 0) {
                        newEnemies.push_back(e);
                    }
                }

                enemies = std::move(newEnemies);
                moveClock.restart();
                updateStatsDisplay();
                return;
            }
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up) || 
            sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) {
            newPos.y = std::max(0, player.pos.y - 1);
            moved = true;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down) || 
                 sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
            newPos.y = std::min(GRID_HEIGHT - 1, player.pos.y + 1);
            moved = true;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) || 
                 sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
            newPos.x = std::max(0, player.pos.x - 1);
            moved = true;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) || 
                 sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
            newPos.x = std::min(GRID_WIDTH - 1, player.pos.x + 1);
            moved = true;
        }

        if (moved && newPos != player.pos) {
            int target = grid[newPos.y][newPos.x];
            if (target == EMPTY || (target >= HEAL && target <= ARMOR ) || target== ROOM || target==KEY) {
                // Always empty old player position
                grid[player.pos.y][player.pos.x] = EMPTY;

                // Collect items if stepping on one
                if ((target >= HEAL && target <= ARMOR) || target == KEY) {
                    collectItem(newPos);
                }

                // Move to the new position
                grid[newPos.y][newPos.x] = PLAYER;
                player.pos = newPos;
                moveClock.restart();
            }
            else if (target == STAIRS) {
                level++;
                generateNewLevel();
                moveClock.restart();
            }
            else if (target == TREASURE) {
                // Check if the player has the key
                bool hasKey = false;
                for (const auto* item : inventory) {
                    if (item->getName() == "Key") {
                        hasKey = true;
                        break;
                    }
                }

                if (hasKey) {
                    // Handle winning the game
                    std::cout << "Congratulations! You found the treasure and won the game!\n";
                    level++; // This will put level at MAX_LEVEL + 1
                    updateWinDisplay();
                } else {
                    std::cout << "You need the key to open the treasure chest!\n";
                }

                moveClock.restart();
            }
            inCombat = isAdjacentToEnemy(player.pos);
            showAttackPrompt = inCombat;
            this->viewManager->getView("enemyStats")->setVisible(inCombat);
            updateStatsDisplay();
        }
    }
    
    void useActiveSkill(int index) {
        auto unlockedSkills = skillTree->getUnlockedSkills();
        if (index >= unlockedSkills.size()) return;
        
        Skill* skill = unlockedSkills[index];
        
        // Only consume uses for skills that have limited uses
        // if (!skill->use()) return;
        
        switch (skill->getType()) {
            case Skill::SkillType::WALL_BREAKER:
                std::cout << "Wall breaker used\n";
                // Implement wall breaking - might need a separate mode
                // to select which wall to break
                if(!skill->getUsesLeft()>0)return;
                // Enter wall breaker mode
                wallBreakerMode = true;
                updateWallBreakerDisplay();
                wallBreakerClock.restart();
                std::cout << "Wall breaker activated. Select a wall to break.\n";
                break;
            // case Skill::SkillType::ENEMY_JUMP:
            //     break;
            // case Skill::SkillType::DOUBLE_JUMP:
            //     break;
            // case Skill::SkillType::TELEPORT:
            //     break;
            default:
                // Passive skills don't need to be used
                break;
        }
        
        updateStatsDisplay();
    }

    void draw(sf::RenderWindow& window) {
        window.clear(sf::Color(30, 30, 40));
        viewManager->update();
        viewManager->draw();
    }
    
    int getLevel() const { return level; }
    bool isGameOver() const { return player.hp <= 0; }
};

int main() {
    auto window = sf::RenderWindow(
        sf::VideoMode(sf::Vector2u(GRID_WIDTH * GRID_SIZE, GRID_HEIGHT * GRID_SIZE)),
        "SFML Labyrinth Game"
    );
    window.setFramerateLimit(60);
    
    Game game(window);
    
    while (window.isOpen())
    {
        std::optional<sf::Event> event;
        while ((event = window.pollEvent()))
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
        }
        
        game.handleInput();
        game.draw(window);
        window.display();
        
    }
    
    return 0;
}