#ifndef MAP_H
#define MAP_H

enum class TileType {
    EMPTY = 0,
    WALL = 1,
    DOT = 2,
    POWER_PELLET = 3,
    GATE = 4  // Enemy gate
};

class Map {
public:
    static const int MAX_WIDTH = 21;
    static const int MAX_HEIGHT = 21;
    static const int NUM_ENEMIES = 3;

    Map();

    void loadStage(int stage);
    bool isWalkable(int x, int y) const;
    bool isWalkableForEnemy(int x, int y, bool isEaten = false) const;
    TileType getTile(int x, int y) const;
    void clearTile(int x, int y);

    int getWidth() const { return width; }
    int getHeight() const { return height; }
    int getDotsRemaining() const { return dotsRemaining; }

    int getPlayerStartX() const { return playerStartX; }
    int getPlayerStartY() const { return playerStartY; }
    int getEnemyStartX(int index) const;
    int getEnemyStartY(int index) const;

    // For enemy pathfinding
    int getHomeX() const { return homeX; }
    int getHomeY() const { return homeY; }

    // Wrap coordinates for tunnels
    int wrapX(int x) const;
    int wrapY(int y) const;

private:
    int tiles[MAX_HEIGHT][MAX_WIDTH];
    int width;
    int height;
    int dotsRemaining;
    int playerStartX, playerStartY;
    int enemyStartX[NUM_ENEMIES], enemyStartY[NUM_ENEMIES];
    int homeX, homeY;  // Center of enemy home

    void countDots();
};

#endif // MAP_H
