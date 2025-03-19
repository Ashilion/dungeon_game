#include "Maze.h"
#include <algorithm>

// Constructor (unchanged)
Maze::Maze(unsigned seed) : rng(seed) {
    grid.resize(GRID_HEIGHT, std::vector<int>(GRID_WIDTH, WALL));
    createRooms();
    generateMaze();
    placeStairs();
}

// generateMaze() (unchanged)
void Maze::generateMaze() {
    int startX = 1;
    int startY = 1;
    while (grid[startY][startX] == ROOM) {
        startX += 2;
        if (startX >= GRID_WIDTH - 1) {
            startX = 1;
            startY += 2;
        }
    }
    grid[startY][startX] = EMPTY;

    std::stack<std::pair<int, int>> cellStack;
    cellStack.push({startX, startY});

    const int dx[] = {2, 0, -2, 0};
    const int dy[] = {0, 2, 0, -2};

    while (!cellStack.empty()) {
        auto [x, y] = cellStack.top();

        std::vector<int> directions;
        for (int dir = 0; dir < 4; dir++) {
            int nx = x + dx[dir];
            int ny = y + dy[dir];

            if (nx > 0 && nx < GRID_WIDTH - 1 && ny > 0 && ny < GRID_HEIGHT - 1 && 
                grid[ny][nx] == WALL) {
                directions.push_back(dir);
            }
        }

        if (!directions.empty()) {
            int dir = directions[std::uniform_int_distribution<int>(0, directions.size() - 1)(rng)];
            int nx = x + dx[dir];
            int ny = y + dy[dir];

            grid[y + dy[dir]/2][x + dx[dir]/2] = EMPTY;
            grid[ny][nx] = EMPTY;

            cellStack.push({nx, ny});
        } else {
            cellStack.pop();
        }
    }

    int extraPassages = (GRID_WIDTH + GRID_HEIGHT) / 10;
    for (int i = 0; i < extraPassages; i++) {
        int x = std::uniform_int_distribution<int>(1, GRID_WIDTH - 2)(rng);
        int y = std::uniform_int_distribution<int>(1, GRID_HEIGHT - 2)(rng);
        if ((x % 2 == 0 || y % 2 == 0) && grid[y][x] != ROOM) {
            grid[y][x] = EMPTY;
        }
    }
}

// Updated createRooms()
void Maze::createRooms() {
    int numRooms = std::uniform_int_distribution<int>(3, 6)(rng);
    
    for (int room = 0; room < numRooms; room++) {
        int roomWidth = std::uniform_int_distribution<int>(3, 7)(rng) * 2 - 1;
        int roomHeight = std::uniform_int_distribution<int>(3, 5)(rng) * 2 - 1;
        
        int maxX = GRID_WIDTH - roomWidth - 2;  // Extra buffer for door connections
        int maxY = GRID_HEIGHT - roomHeight - 2;
        
        if (maxX < 3 || maxY < 3) continue;
        
        int roomX = std::uniform_int_distribution<int>(3, maxX)(rng);
        int roomY = std::uniform_int_distribution<int>(3, maxY)(rng);
        
        if (roomX % 2 == 0) roomX++;
        if (roomY % 2 == 0) roomY++;
        
        bool canPlaceRoom = true;
        for (int y = roomY - 1; y <= roomY + roomHeight + 1; y++) {
            for (int x = roomX - 1; x <= roomX + roomWidth + 1; x++) {
                if (y < 0 || y >= GRID_HEIGHT || x < 0 || x >= GRID_WIDTH || grid[y][x] != WALL) {
                    canPlaceRoom = false;
                    break;
                }
            }
            if (!canPlaceRoom) break;
        }
        
        if (!canPlaceRoom) continue;
        
        // Carve out the room
        for (int y = roomY; y < roomY + roomHeight; y++) {
            for (int x = roomX; x < roomX + roomWidth; x++) {
                grid[y][x] = ROOM;
            }
        }
        
        // Create one entrance and one exit on opposite sides
        std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>> doorPositions; // {door pos, connect pos}
        
        // North wall: door at (x, roomY-1), connect point at (x, roomY-2)
        int doorX = roomX + std::uniform_int_distribution<int>(0, roomWidth - 1)(rng) / 2 * 2 + 1;
        if (roomY >= 2) {
            doorPositions.push_back({{doorX, roomY - 1}, {doorX, roomY - 2}});
        }
        
        // South wall: door at (x, roomY+roomHeight), connect point at (x, roomY+roomHeight+1)
        doorX = roomX + std::uniform_int_distribution<int>(0, roomWidth - 1)(rng) / 2 * 2 + 1;
        if (roomY + roomHeight + 1 < GRID_HEIGHT - 1) {
            doorPositions.push_back({{doorX, roomY + roomHeight}, {doorX, roomY + roomHeight + 1}});
        }
        
        // West wall: door at (roomX-1, y), connect point at (roomX-2, y)
        int doorY = roomY + std::uniform_int_distribution<int>(0, roomHeight - 1)(rng) / 2 * 2 + 1;
        if (roomX >= 2) {
            doorPositions.push_back({{roomX - 1, doorY}, {roomX - 2, doorY}});
        }
        
        // East wall: door at (roomX+roomWidth, y), connect point at (roomX+roomWidth+1, y)
        doorY = roomY + std::uniform_int_distribution<int>(0, roomHeight - 1)(rng) / 2 * 2 + 1;
        if (roomX + roomWidth + 1 < GRID_WIDTH - 1) {
            doorPositions.push_back({{roomX + roomWidth, doorY}, {roomX + roomWidth + 1, doorY}});
        }
        
        if (doorPositions.size() < 2) continue; // Need at least two valid door positions
        
        // Choose two opposite sides for entrance and exit
        int side1 = std::uniform_int_distribution<int>(0, doorPositions.size() - 1)(rng);
        int side2 = (side1 + (doorPositions.size() / 2)) % doorPositions.size(); // Try to get opposite side
        
        // Ensure they're different
        while (side2 == side1 && doorPositions.size() > 1) {
            side2 = std::uniform_int_distribution<int>(0, doorPositions.size() - 1)(rng);
        }
        
        auto [entrancePos, entranceConnect] = doorPositions[side1];
        auto [exitPos, exitConnect] = doorPositions[side2];
        
        // Place doors and ensure connect points are empty
        grid[entrancePos.second][entrancePos.first] = EMPTY;
        grid[entranceConnect.second][entranceConnect.first] = EMPTY;
        
        grid[exitPos.second][exitPos.first] = EMPTY;
        grid[exitConnect.second][exitConnect.first] = EMPTY;
    }
}

// placeStairs() (unchanged)
void Maze::placeStairs() {
    int sx, sy;
    do {
        sx = std::uniform_int_distribution<int>(GRID_WIDTH / 2, GRID_WIDTH - 3)(rng);
        sy = std::uniform_int_distribution<int>(GRID_HEIGHT / 2, GRID_HEIGHT - 3)(rng);
    } while (grid[sy][sx] != EMPTY && grid[sy][sx] != ROOM);

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