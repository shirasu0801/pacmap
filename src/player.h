#ifndef PLAYER_H
#define PLAYER_H

class Map;

enum class Direction {
    NONE = -1,
    UP = 0,
    RIGHT = 1,
    DOWN = 2,
    LEFT = 3
};

class Player {
public:
    Player();

    void init(int startX, int startY);
    void update(const Map& map);
    void setDirection(Direction dir);
    bool canMove(const Map& map, Direction dir) const;

    // Tile position (for collision)
    int getTileX() const { return tileX; }
    int getTileY() const { return tileY; }

    // Pixel position (for smooth rendering)
    float getX() const { return x; }
    float getY() const { return y; }
    int getDirection() const { return static_cast<int>(currentDir); }

    bool isPowerMode() const { return powerTimer > 0; }
    int getPowerTimer() const { return powerTimer; }
    void activatePowerMode();
    bool isInvincible() const { return spawnProtection > 0; }  // スポーン保護中か

private:
    float x, y;           // Smooth pixel position
    int tileX, tileY;     // Current tile position
    Direction currentDir;
    Direction nextDir;
    int powerTimer;
    int spawnProtection;  // スポーン後の無敵時間

    static constexpr float MOVE_SPEED = 0.08f;  // Tiles per frame (reduced for better control)
    static const int POWER_DURATION = 420;       // 7 seconds at 60fps
    static const int SPAWN_PROTECTION_TIME = 120; // 2 seconds at 60fps
};

#endif // PLAYER_H
