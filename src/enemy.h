#ifndef ENEMY_H
#define ENEMY_H

class Map;
class Player;

enum class EnemyState {
    WANDER,     // Wandering randomly
    CHASE,      // Chasing player
    FRIGHTENED, // Running away (power pellet active)
    EATEN       // Was eaten, returning to start
};

class Enemy {
public:
    Enemy();

    void init(int startX, int startY, float speed, const Map* mapRef);
    void update(const Map& map, const Player& player);
    void setFrightened(bool frightened);
    void setCanChase(bool canChase);  // 追跡を許可するか
    void eat();

    // Tile position (for collision)
    int getTileX() const { return tileX; }
    int getTileY() const { return tileY; }

    // Pixel position (for smooth rendering)
    float getX() const { return x; }
    float getY() const { return y; }
    int getState() const { return static_cast<int>(state); }
    bool isEaten() const { return state == EnemyState::EATEN; }
    bool isRecovering() const { return recoveryTimer > 0; }  // 復活直後の保護

private:
    void chooseNextDirection(const Map& map, const Player& player);
    void moveTowardsTarget(const Map& map);
    int getDistanceToPlayer(const Player& player) const;
    int findBestDirectionToTarget(const Map& map, int targetX, int targetY, bool isEatenState);

    float x, y;           // Smooth pixel position
    float prevX, prevY;   // Previous position for validation
    int tileX, tileY;     // Current tile position
    int targetTileX, targetTileY;  // Target tile
    int startX, startY;   // Start position for respawn
    int direction;        // Current direction (0-3)
    float moveSpeed;      // Movement speed
    EnemyState state;
    bool needNewDirection;
    bool allowedToChase;  // この敵が追跡を許可されているか
    int eatenTimer;       // 食べられた後の復活タイマー
    int recoveryTimer;    // 復活後の保護タイマー
    const Map* gameMap;   // Reference to map for pathfinding

    static const int CHASE_THRESHOLD = 8; // Manhattan distance
    static const int EATEN_DURATION = 180; // 3 seconds at 60fps
    static const int RECOVERY_DURATION = 60; // 1 second recovery after returning
};

#endif // ENEMY_H
