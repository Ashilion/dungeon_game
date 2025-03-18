
#include "Maze.h"

// Constructor
Maze::Maze(unsigned seed) : rng(seed) {
    grid.resize(GRID_HEIGHT, std::vector<int>(GRID_WIDTH, WALL));
    generateMaze();
    placeStairs();
}

// Generate the maze using recursive backtracking
void Maze::generateMaze() {
    // Start with a grid full of walls
    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            grid[y][x] = WALL;
        }
    }

    // Create maze using recursive backtracking
    int startX = 1;
    int startY = 1;
    grid[startY][startX] = EMPTY;

    std::stack<std::pair<int, int>> cellStack;
    cellStack.push({startX, startY});

    // Directions: right, down, left, up
    const int dx[] = {2, 0, -2, 0};
    const int dy[] = {0, 2, 0, -2};

    while (!cellStack.empty()) {
        auto [x, y] = cellStack.top();

        // Find unvisited neighbors
        std::vector<int> directions;
        for (int dir = 0; dir < 4; dir++) {
            int nx = x + dx[dir];
            int ny = y + dy[dir];

            if (nx > 0 && nx < GRID_WIDTH - 1 && ny > 0 && ny < GRID_HEIGHT - 1 && grid[ny][nx] == WALL) {
                directions.push_back(dir);
            }
        }

        if (!directions.empty()) {
            // Choose random direction
            int dir = directions[std::uniform_int_distribution<int>(0, directions.size() - 1)(rng)];
            int nx = x + dx[dir];
            int ny = y + dy[dir];

            // Carve passage
            grid[y + dy[dir]/2][x + dx[dir]/2] = EMPTY;
            grid[ny][nx] = EMPTY;

            cellStack.push({nx, ny});
        } else {
            cellStack.pop();
        }
    }

    // Add a few random passages to make it more interesting
    int extraPassages = (GRID_WIDTH + GRID_HEIGHT) / 10;
    for (int i = 0; i < extraPassages; i++) {
        int x = std::uniform_int_distribution<int>(1, GRID_WIDTH - 2)(rng);
        int y = std::uniform_int_distribution<int>(1, GRID_HEIGHT - 2)(rng);
        if (x % 2 == 0 || y % 2 == 0) { // Only remove walls
            grid[y][x] = EMPTY;
        }
    }
}

// Place stairs in the maze
void Maze::placeStairs() {
    // Find a suitable location for stairs (empty cell away from start)
    int sx, sy;
    do {
        sx = std::uniform_int_distribution<int>(GRID_WIDTH / 2, GRID_WIDTH - 3)(rng);
        sy = std::uniform_int_distribution<int>(GRID_HEIGHT / 2, GRID_HEIGHT - 3)(rng);
    } while (grid[sy][sx] != EMPTY);

    grid[sy][sx] = STAIRS;
}

// Get the grid (const version)
const std::vector<std::vector<int>>& Maze::getGrid() const {
    return grid;
}

// Get the grid (non-const version)
std::vector<std::vector<int>>& Maze::getGrid() {
    return grid;
}