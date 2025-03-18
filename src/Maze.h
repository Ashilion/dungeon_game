#ifndef MAZE_H
#define MAZE_H


#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <random>
#include <stack>
#include <chrono>

const int GRID_SIZE = 38; // Size of each cell in pixels
const int GRID_WIDTH = 41; // Number of cells horizontally (odd for maze generation)
const int GRID_HEIGHT = 28; // Number of cells vertically (odd for maze generation)
const float MOVE_DELAY = 0.15f; // Seconds between moves

// Game entity types
enum EntityType {
    EMPTY = 0,
    WALL = 1,
    PLAYER = 2,
    STAIRS = 3,
    ENEMY = 4,
    HEAL = 5,
    WEAPON = 6,
    ARMOR = 7
};

class Maze {
    public:
        // Constructor
        Maze(unsigned seed);
    
        // Public methods
        void generateMaze();
        void placeStairs();
        const std::vector<std::vector<int>>& getGrid() const;
        std::vector<std::vector<int>>& getGrid();
        void addRooms();
        void recursiveBacktrack(int x, int y);
    
    private:
        // Member variables
        std::vector<std::vector<int>> grid;
        std::mt19937 rng;
    
        // Helper methods (if needed)
        // Example: bool isValidCell(int x, int y);
    };
    
#endif // MAZE_H