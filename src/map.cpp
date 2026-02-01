#include "map.h"

// Stage layouts (21x21 max)
// 0=empty, 1=wall, 2=dot, 3=power pellet, 4=gate
// NO warp tunnels - simple closed maze

// Stage 1: Simple maze (no dead ends, with warp tunnels)
static const int STAGE1_WIDTH = 19;
static const int STAGE1_HEIGHT = 21;
static const int STAGE1[21][19] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,2,2,2,2,2,2,2,2,1,2,2,2,2,2,2,2,2,1},
    {1,3,1,2,1,1,1,1,2,1,2,1,1,1,1,2,1,3,1},
    {1,2,1,2,2,2,2,2,2,2,2,2,2,2,2,2,1,2,1},
    {1,2,1,2,1,2,1,1,1,1,1,1,1,2,1,2,1,2,1},
    {1,2,2,2,1,2,2,2,2,1,2,2,2,2,1,2,2,2,1},
    {1,1,1,2,1,1,1,1,2,1,2,1,1,1,1,2,1,1,1},
    {1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1},
    {1,2,1,1,1,2,1,1,1,1,1,1,1,2,1,1,1,2,1},
    {1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1},
    {0,2,1,1,1,1,1,1,2,1,2,1,1,1,1,1,1,2,0},
    {1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1},
    {1,2,1,1,1,2,1,1,1,1,1,1,1,2,1,1,1,2,1},
    {1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1},
    {1,1,1,2,1,1,1,1,2,1,2,1,1,1,1,2,1,1,1},
    {1,2,2,2,1,2,2,2,2,1,2,2,2,2,1,2,2,2,1},
    {1,2,1,2,1,2,1,1,1,1,1,1,1,2,1,2,1,2,1},
    {1,2,1,2,2,2,2,2,2,2,2,2,2,2,2,2,1,2,1},
    {1,3,1,2,1,1,1,1,2,1,2,1,1,1,1,2,1,3,1},
    {1,2,2,2,2,2,2,2,2,1,2,2,2,2,2,2,2,2,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

// Stage 2: Medium maze (no dead ends, with warp tunnels)
static const int STAGE2_WIDTH = 19;
static const int STAGE2_HEIGHT = 21;
static const int STAGE2[21][19] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,2,2,2,2,2,2,2,2,1,2,2,2,2,2,2,2,2,1},
    {1,2,1,1,1,1,2,1,2,1,2,1,2,1,1,1,1,2,1},
    {1,3,2,2,2,2,2,1,2,2,2,1,2,2,2,2,2,3,1},
    {1,1,1,2,1,1,2,1,2,1,2,1,2,1,1,2,1,1,1},
    {1,2,2,2,2,2,2,2,2,1,2,2,2,2,2,2,2,2,1},
    {1,2,1,1,1,1,1,1,2,1,2,1,1,1,1,1,1,2,1},
    {1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1},
    {1,2,1,2,1,1,2,1,1,1,1,1,2,1,1,2,1,2,1},
    {1,2,1,2,2,2,2,2,2,2,2,2,2,2,2,2,1,2,1},
    {0,2,1,2,1,1,1,1,2,1,2,1,1,1,1,2,1,2,0},
    {1,2,1,2,2,2,2,2,2,2,2,2,2,2,2,2,1,2,1},
    {1,2,1,2,1,1,2,1,1,1,1,1,2,1,1,2,1,2,1},
    {1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1},
    {1,2,1,1,1,1,1,1,2,1,2,1,1,1,1,1,1,2,1},
    {1,2,2,2,2,2,2,2,2,1,2,2,2,2,2,2,2,2,1},
    {1,1,1,2,1,1,2,1,2,1,2,1,2,1,1,2,1,1,1},
    {1,3,2,2,2,2,2,1,2,2,2,1,2,2,2,2,2,3,1},
    {1,2,1,1,1,1,2,1,2,1,2,1,2,1,1,1,1,2,1},
    {1,2,2,2,2,2,2,2,2,1,2,2,2,2,2,2,2,2,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

// Stage 3: Complex maze (no dead ends, with warp tunnels)
static const int STAGE3_WIDTH = 19;
static const int STAGE3_HEIGHT = 21;
static const int STAGE3[21][19] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,2,2,2,2,2,2,2,2,1,2,2,2,2,2,2,2,2,1},
    {1,3,1,2,1,2,1,1,2,1,2,1,1,2,1,2,1,3,1},
    {1,2,1,2,1,2,2,2,2,2,2,2,2,2,1,2,1,2,1},
    {1,2,2,2,1,2,1,1,2,1,2,1,1,2,1,2,2,2,1},
    {1,1,1,2,1,2,2,2,2,1,2,2,2,2,1,2,1,1,1},
    {1,2,2,2,2,2,1,1,2,1,2,1,1,2,2,2,2,2,1},
    {1,2,1,1,1,2,2,2,2,2,2,2,2,2,1,1,1,2,1},
    {1,2,2,2,2,2,1,1,2,1,2,1,1,2,2,2,2,2,1},
    {1,1,1,2,1,2,2,2,2,1,2,2,2,2,1,2,1,1,1},
    {0,2,2,2,1,2,1,1,2,1,2,1,1,2,1,2,2,2,0},
    {1,1,1,2,1,2,2,2,2,1,2,2,2,2,1,2,1,1,1},
    {1,2,2,2,2,2,1,1,2,1,2,1,1,2,2,2,2,2,1},
    {1,2,1,1,1,2,2,2,2,2,2,2,2,2,1,1,1,2,1},
    {1,2,2,2,2,2,1,1,2,1,2,1,1,2,2,2,2,2,1},
    {1,1,1,2,1,2,2,2,2,1,2,2,2,2,1,2,1,1,1},
    {1,2,2,2,1,2,1,1,2,1,2,1,1,2,1,2,2,2,1},
    {1,2,1,2,1,2,2,2,2,2,2,2,2,2,1,2,1,2,1},
    {1,3,1,2,1,2,1,1,2,1,2,1,1,2,1,2,1,3,1},
    {1,2,2,2,2,2,2,2,2,1,2,2,2,2,2,2,2,2,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

Map::Map() : width(0), height(0), dotsRemaining(0),
             playerStartX(9), playerStartY(15),
             homeX(9), homeY(9) {
    // Initialize enemy positions to corners (far from player)
    enemyStartX[0] = 1;  enemyStartY[0] = 1;
    enemyStartX[1] = 17; enemyStartY[1] = 1;
    enemyStartX[2] = 1;  enemyStartY[2] = 19;
}

void Map::loadStage(int stage) {
    const int (*stageData)[19] = nullptr;

    switch (stage) {
        case 1:
            stageData = STAGE1;
            width = STAGE1_WIDTH;
            height = STAGE1_HEIGHT;
            break;
        case 2:
            stageData = STAGE2;
            width = STAGE2_WIDTH;
            height = STAGE2_HEIGHT;
            break;
        case 3:
        default:
            stageData = STAGE3;
            width = STAGE3_WIDTH;
            height = STAGE3_HEIGHT;
            break;
    }

    // Copy map data
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            tiles[y][x] = stageData[y][x];
        }
    }

    // Set start positions - player in center-bottom
    playerStartX = 9;
    playerStartY = 15;

    // Enemy start positions - corners, far from player at (9, 15)
    enemyStartX[0] = 1;  enemyStartY[0] = 1;   // Top-left
    enemyStartX[1] = 17; enemyStartY[1] = 1;   // Top-right
    enemyStartX[2] = 1;  enemyStartY[2] = 19;  // Bottom-left

    // Home position for reference
    homeX = 9;
    homeY = 9;

    countDots();
}

bool Map::isWalkable(int x, int y) const {
    // Simple bounds check - no wrapping
    if (x < 0 || x >= width || y < 0 || y >= height) {
        return false;
    }
    TileType tile = static_cast<TileType>(tiles[y][x]);
    return tile != TileType::WALL && tile != TileType::GATE;
}

bool Map::isWalkableForEnemy(int x, int y, bool isEaten) const {
    // Simple bounds check - no wrapping for enemies
    if (x < 0 || x >= width || y < 0 || y >= height) {
        return false;
    }
    // Enemies cannot use warp tunnels (edge tiles)
    if (x == 0 || x == width - 1) {
        TileType tile = static_cast<TileType>(tiles[y][x]);
        if (tile == TileType::EMPTY) {
            return false;  // Block warp tunnel for enemies
        }
    }
    TileType tile = static_cast<TileType>(tiles[y][x]);
    // Enemies can pass through gate
    return tile != TileType::WALL;
}

int Map::wrapX(int x) const {
    // Wrap around horizontally for warp tunnels
    if (x < 0) return width - 1;
    if (x >= width) return 0;
    return x;
}

int Map::wrapY(int y) const {
    // No wrapping - clamp to bounds
    if (y < 0) return 0;
    if (y >= height) return height - 1;
    return y;
}

TileType Map::getTile(int x, int y) const {
    if (x < 0 || x >= width || y < 0 || y >= height) {
        return TileType::WALL;  // Out of bounds = wall
    }
    return static_cast<TileType>(tiles[y][x]);
}

void Map::clearTile(int x, int y) {
    if (x >= 0 && x < width && y >= 0 && y < height) {
        TileType tile = static_cast<TileType>(tiles[y][x]);
        if (tile == TileType::DOT || tile == TileType::POWER_PELLET) {
            tiles[y][x] = static_cast<int>(TileType::EMPTY);
            dotsRemaining--;
        }
    }
}

int Map::getEnemyStartX(int index) const {
    if (index >= 0 && index < NUM_ENEMIES) {
        return enemyStartX[index];
    }
    return 1;  // Default to corner
}

int Map::getEnemyStartY(int index) const {
    if (index >= 0 && index < NUM_ENEMIES) {
        return enemyStartY[index];
    }
    return 1;  // Default to corner
}

void Map::countDots() {
    dotsRemaining = 0;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            TileType tile = static_cast<TileType>(tiles[y][x]);
            if (tile == TileType::DOT || tile == TileType::POWER_PELLET) {
                dotsRemaining++;
            }
        }
    }
}
