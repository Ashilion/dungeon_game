#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <random>
#include <stack>
#include <chrono>
#include "Maze.h"
#include "SkillTree.h" 

#define ENEMY_COUNT 5
#define ITEM_COUNT 3
#define MOVE_DELAY 0.1f
#define ATTACK_DELAY 0.5f
#define PLAYER_BASE_HP 100
#define PLAYER_BASE_ATTACK 15
#define VISION_RADIUS 8  // Rayon de vision en nombre de cases  


struct EnemyType {
    int baseHp;
    int baseAttack;
    sf::Color color;
    std::string name;
};

struct Entity {
    sf::Vector2i pos;
    int hp;
    int attack;
    int type;  // Used for enemies to determine their type
    Entity(sf::Vector2i p, int h, int a, int t = 0) : pos(p), hp(h), attack(a), type(t) {}
};

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
    sf::Text playerStatsText;
    sf::Text enemyStatsText;
    sf::Texture playerTexture;
    sf::Texture wallTexture;
    sf::Texture stairsTexture;
    sf::Texture healTexture;
    sf::Texture weaponTexture;
    sf::Texture armorTexture;
    sf::Texture floorTexture;
    std::vector<sf::Texture> enemyTextures; // One for each enemy type
    SkillTree skillTree;
    int skillPoints;
    bool showSkillTree;
    sf::Clock inputClock;
    bool wallBreakerMode = false;
    sf::Clock wallBreakerClock;

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
    Game() : player(sf::Vector2i(0, 0), PLAYER_BASE_HP, PLAYER_BASE_ATTACK), 
             level(1), inCombat(false), showAttackPrompt(false), enemyStatsText(font, "0", 10), playerStatsText(font, "0", 10),skillPoints(0), showSkillTree(false) {
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        rng.seed(seed);
        
        if (!font.openFromFile("assets/arial.ttf")) {
            std::cerr << "Error loading font\n";
        }
        
        playerStatsText.setFont(font);
        playerStatsText.setCharacterSize(16);
        playerStatsText.setFillColor(sf::Color::White);
        playerStatsText.setPosition(sf::Vector2f(10.f, 10.f));
        
        enemyStatsText.setFont(font);
        enemyStatsText.setCharacterSize(16);
        enemyStatsText.setFillColor(sf::Color::White);
        enemyStatsText.setPosition(sf::Vector2f(200.f, 10.f));
        
        // Load all textures
        if (!playerTexture.loadFromFile("assets/player2.png")) {
            std::cerr << "Error loading player texture\n";
        }
        if (!wallTexture.loadFromFile("assets/wall.jpg")) {  // Add your wall image
            std::cerr << "Error loading wall texture\n";
        }
        if (!stairsTexture.loadFromFile("assets/stairs.png")) {  // Add your stairs image
            std::cerr << "Error loading stairs texture\n";
        }
        if (!healTexture.loadFromFile("assets/heal.png")) {  // Add your heal item image
            std::cerr << "Error loading heal texture\n";
        }
        if (!weaponTexture.loadFromFile("assets/weapon.png")) {  // Add your weapon image
            std::cerr << "Error loading weapon texture\n";
        }
        if (!armorTexture.loadFromFile("assets/armor.png")) {  // Add your armor image
            std::cerr << "Error loading armor texture\n";
        }
        if (!floorTexture.loadFromFile("assets/floor.jpg")) {  // Add your armor image
            std::cerr << "Error loading armor texture\n";
        }
        
        
        // Load enemy textures
        enemyTextures.resize(3);
        if (!enemyTextures[0].loadFromFile("assets/goblin.png")) {  // Add your goblin image
            std::cerr << "Error loading goblin texture\n";
        }
        if (!enemyTextures[1].loadFromFile("assets/orc.png")) {  // Add your orc image
            std::cerr << "Error loading orc texture\n";
        }
        if (!enemyTextures[2].loadFromFile("assets/troll.png")) {  // Add your troll image
            std::cerr << "Error loading troll texture\n";
        }
        
        skillTree.setPosition(sf::Vector2f(GRID_SIZE * 2, GRID_SIZE * 2), 
                         sf::Vector2f(GRID_SIZE * (GRID_WIDTH - 4), GRID_SIZE * (GRID_HEIGHT - 4)));
    
        // Set callback for when a skill is selected
        skillTree.setCallback([this](Skill* skill) {
            applySkill(skill);
            skillPoints--;
            updateStatsDisplay();
            showSkillTree = false;
        });

        generateNewLevel();
    }

    void applySkill(Skill* skill) {
        skill->unlock();
        
        switch (skill->getType()) {
            case Skill::SkillType::HEALTH_BOOST:
                player.hp += 25;
                break;
            case Skill::SkillType::ATTACK_BOOST:
                player.attack += 10;
                break;
            case Skill::SkillType::VISION_BOOST:
                // Increase VISION_RADIUS by 3
                // You'll need to make VISION_RADIUS a class member instead of a #define
                // to modify it at runtime
                break;
            // Other skill types will be used when the player activates them
            default:
                break;
        }
    }
    
    void generateNewLevel() {
        Maze maze(rng());
        grid = maze.getGrid();
        
        // Place player
        do {
            player.pos.x = std::uniform_int_distribution<int>(1, GRID_WIDTH / 4)(rng);
            player.pos.y = std::uniform_int_distribution<int>(1, GRID_HEIGHT / 4)(rng);
        } while (grid[player.pos.y][player.pos.x] != EMPTY);
        grid[player.pos.y][player.pos.x] = PLAYER;
        
        // Place enemies with level scaling
        enemies.clear();
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
        
        // Place items
        for (int i = 0; i < ITEM_COUNT; i++) {
            sf::Vector2i itemPos;
            int itemType = std::uniform_int_distribution<int>(0, 2)(rng); // 0: heal, 1: weapon, 2: armor
            do {
                itemPos.x = std::uniform_int_distribution<int>(1, GRID_WIDTH - 2)(rng);
                itemPos.y = std::uniform_int_distribution<int>(1, GRID_HEIGHT - 2)(rng);
            } while (grid[itemPos.y][itemPos.x] != EMPTY);
            
            grid[itemPos.y][itemPos.x] = HEAL + itemType;
        }

        if (level > 1) {
            skillPoints++;
            
            // Offer random skills
            skillTree.selectRandomSkills(3); // Offer 3 skills to choose from
            showSkillTree = true;
        }
        
        updateStatsDisplay();
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
        std::string playerStats = "Player\nHP: " + std::to_string(player.hp) +
                            "\nAttack: " + std::to_string(player.attack) +
                            "\nLevel: " + std::to_string(level);
    
        // Add skill points if any
        if (skillPoints > 0) {
            playerStats += "\nSkill Points: " + std::to_string(skillPoints);
        }
        
        playerStatsText.setString(playerStats);
        
        std::string enemyStats = "Enemy\n";
        if (inCombat) {
            for (const auto& enemy : enemies) {
                if (std::abs(enemy.pos.x - player.pos.x) + std::abs(enemy.pos.y - player.pos.y) == 1) {
                    enemyStats += "Type: " + enemyTypes[enemy.type].name +
                                "\nHP: " + std::to_string(enemy.hp) +
                                "\nAttack: " + std::to_string(enemy.attack);
                    break;
                }
            }
        } else {
            enemyStats += "No enemy nearby";
        }
        enemyStatsText.setString(enemyStats);
    }
    
    void handleInput() {
        if (moveClock.getElapsedTime().asSeconds() < MOVE_DELAY) return;
        if (player.hp <= 0) return;
        
        // Handle skill tree if it's active
        if (showSkillTree) {
            skillTree.handleInput(inputClock, MOVE_DELAY);
            if (inputClock.getElapsedTime().asSeconds() >= MOVE_DELAY) {
                inputClock.restart();
            }
            return;
        }

        if (wallBreakerMode) {
            // Time limit for selecting a wall (5 seconds)
            if (wallBreakerClock.getElapsedTime().asSeconds() > 5.0f) {
                wallBreakerMode = false;
                return;
            }
            
            sf::Vector2i targetPos = player.pos;
            bool selected = false;
            
            // Allow selecting adjacent walls
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) {
                targetPos.y = std::max(0, player.pos.y - 1);
                selected = true;
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
                targetPos.y = std::min(GRID_HEIGHT - 1, player.pos.y + 1);
                selected = true;
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
                targetPos.x = std::max(0, player.pos.x - 1);
                selected = true;
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
                targetPos.x = std::min(GRID_WIDTH - 1, player.pos.x + 1);
                selected = true;
            }
            
            // Cancel with Escape
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) {
                wallBreakerMode = false;
                return;
            }
            
            if (selected && grid[targetPos.y][targetPos.x] == WALL) {
                // Break the wall!
                grid[targetPos.y][targetPos.x] = EMPTY;
                wallBreakerMode = false;
                
                // Consume a use from the Wall Breaker skill
                for (auto& skill : skillTree.getUnlockedSkills()) {
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
        
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) {
            newPos.y = std::max(0, player.pos.y - 1);
            moved = true;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
            newPos.y = std::min(GRID_HEIGHT - 1, player.pos.y + 1);
            moved = true;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
            newPos.x = std::max(0, player.pos.x - 1);
            moved = true;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
            newPos.x = std::min(GRID_WIDTH - 1, player.pos.x + 1);
            moved = true;
        }
        
        if (moved && newPos != player.pos) {
            int target = grid[newPos.y][newPos.x];
            if (target == EMPTY || target >= HEAL) {
                grid[player.pos.y][player.pos.x] = EMPTY;
                if (target == HEAL) {
                    player.hp += 25 + level * 5;  // Scaling heal amount
                }
                else if (target == WEAPON) {
                    player.attack += 5 + level * 2;  // Scaling attack boost
                }
                else if (target == ARMOR) {
                    player.hp += 20 + level * 5;  // Scaling hp boost
                }
                grid[newPos.y][newPos.x] = PLAYER;
                player.pos = newPos;
                moveClock.restart();
            }
            else if (target == STAIRS) {
                level++;
                generateNewLevel();
                moveClock.restart();
            }
            inCombat = isAdjacentToEnemy(player.pos);
            showAttackPrompt = inCombat;
            updateStatsDisplay();
        }
    }
    
    void useActiveSkill(int index) {
        auto unlockedSkills = skillTree.getUnlockedSkills();
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
                wallBreakerClock.restart();
                std::cout << "Wall breaker activated. Select a wall to break.\n";
                break;
            // case Skill::SkillType::ENEMY_JUMP:
            //     // Implement jumping over an enemy
            //     break;
            // case Skill::SkillType::DOUBLE_JUMP:
            //     // Implement double jump
            //     break;
            // case Skill::SkillType::TELEPORT:
            //     // Implement teleport - might need a separate mode
            //     // to select destination
            //     break;
            default:
                // Passive skills don't need to be used
                break;
        }
        
        updateStatsDisplay();
    }

    void draw(sf::RenderWindow& window) {
        window.clear(sf::Color(30, 30, 40));
        
        sf::Sprite sprite(playerTexture); // Sprite réutilisable
        
        for (int y = 0; y < GRID_HEIGHT; y++) {
            for (int x = 0; x < GRID_WIDTH; x++) {
                // Calculer la distance entre la case et le joueur (distance de Manhattan)
                int dx = std::abs(x - player.pos.x);
                int dy = std::abs(y - player.pos.y);
                float distance = static_cast<float>(dx + dy); // Distance de Manhattan
                
                // Appliquer une transparence progressive
                float alpha = 255.f; // Opacité complète par défaut
                if (distance > VISION_RADIUS / 2) {
                    // Réduire l'opacité progressivement entre la moitié du rayon et le bord
                    alpha = 255.f * (VISION_RADIUS - distance) / (VISION_RADIUS / 2);
                    alpha = std::max(0.f, std::min(255.f, alpha)); // Limiter entre 0 et 255
                }
                
                switch (grid[y][x]) {
                    case EMPTY:
                        sprite = createSprite(floorTexture);
                        break; // Skip empty spaces
                    case WALL:
                        sprite = createSprite(wallTexture);
                        break;
                    case PLAYER:
                        sprite = createSprite(playerTexture);
                        break;
                    case STAIRS:
                        sprite = createSprite(stairsTexture);
                        break;
                    case ENEMY:
                        for (const auto& e : enemies) {
                            if (e.pos.x == x && e.pos.y == y) {
                                sprite = createSprite(enemyTextures[e.type]);
                                break;
                            }
                        }
                        break;
                    case HEAL:
                        sprite = createSprite(healTexture);
                        break;
                    case WEAPON:
                        sprite = createSprite(weaponTexture);
                        break;
                    case ARMOR:
                        sprite = createSprite(armorTexture);
                        break;
                    case ROOM:
                        sprite = createSprite(floorTexture);
                        break;
                }
                
                // Appliquer la position et la transparence
                sprite.setPosition(sf::Vector2f(x * GRID_SIZE, y * GRID_SIZE));
                sprite.setColor(sf::Color(255, 255, 255, static_cast<uint8_t>(alpha)));
                window.draw(sprite);
            }
        }
        
        // Afficher les textes (stats, prompt) sans brouillard
        if (showAttackPrompt) {
            sf::Text attackPrompt(font, "Press SPACE to attack", 16);
            attackPrompt.setPosition(sf::Vector2f(GRID_SIZE * GRID_WIDTH - 150.f, 10.f));
            attackPrompt.setFillColor(sf::Color::Red);
            window.draw(attackPrompt);
        }
        
        window.draw(playerStatsText);
        window.draw(enemyStatsText);

        auto unlockedSkills = skillTree.getUnlockedSkills();
        if (!unlockedSkills.empty()) {
            sf::Text skillListText(font);
            skillListText.setCharacterSize(16);
            skillListText.setFillColor(sf::Color::White);
            
            std::string skillList = "Skills: ";
            for (size_t i = 0; i < unlockedSkills.size(); i++) {
                skillList += std::to_string(i+1) + ":" + unlockedSkills[i]->getName() + " (" + std::to_string(unlockedSkills[i]->getUsesLeft()) + "charges )";                                                                   
                if (i < unlockedSkills.size() - 1) {
                    skillList += ", ";
                }
            }
            
            skillListText.setString(skillList);
            skillListText.setPosition(sf::Vector2f(10.f, GRID_SIZE * GRID_HEIGHT - 150.f));
            window.draw(skillListText);
    }
    
    // Add skill points display
    if (skillPoints > 0) {
        sf::Text skillPointsText(font);
        skillPointsText.setCharacterSize(16);
        skillPointsText.setFillColor(sf::Color::Yellow);
        skillPointsText.setString("Skill Points: " + std::to_string(skillPoints) + " (Press T)");
        skillPointsText.setPosition(sf::Vector2f(GRID_SIZE * GRID_WIDTH - 200.f,10));
        window.draw(skillPointsText);
    }
    
    // Draw skill tree if active
        if (showSkillTree) {
            skillTree.draw(window);
        }
        
        if (wallBreakerMode) {
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
            
            // Add instruction text
            sf::Text instructions(font);
            instructions.setFont(font);
            instructions.setCharacterSize(16);
            instructions.setString("Select a wall to break (arrow keys), or ESC to cancel");
            instructions.setFillColor(sf::Color::Yellow);
            instructions.setPosition(sf::Vector2f( 10, GRID_SIZE * GRID_HEIGHT - 100));
            window.draw(instructions);
        }
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
    
    Game game;
    
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
        
        if (game.isGameOver()) {
            window.close();
        }
    }
    
    return 0;
}