#ifndef GAME_H
#define GAME_H

#include "player.h"
#include "enemy.h"
#include "map.h"

enum class GameState {
    TITLE,
    PLAYING,
    STAGE_CLEAR,
    GAME_CLEAR,
    GAME_OVER
};

class Game {
public:
    static const int NUM_ENEMIES = 3;  // 敵の数

    Game();

    void init();
    void update();
    void setInput(int direction);

    // Getters for JS
    int getState() const { return static_cast<int>(state); }
    int getScore() const { return score; }
    int getLives() const { return lives; }
    int getStage() const { return currentStage; }
    int getNumEnemies() const { return NUM_ENEMIES; }

    // Player getters (float for smooth rendering)
    float getPlayerX() const;
    float getPlayerY() const;
    int getPlayerTileX() const;
    int getPlayerTileY() const;
    int getPlayerDir() const;
    bool isPowerMode() const;
    int getPowerTimer() const;
    bool isPlayerInvincible() const;

    // Enemy getters (float for smooth rendering)
    float getEnemyX(int index) const;
    float getEnemyY(int index) const;
    int getEnemyState(int index) const;
    bool isEnemyRecovering(int index) const;

    // Map getters
    int getTile(int x, int y) const;
    int getMapWidth() const;
    int getMapHeight() const;

    void startGame();

private:
    void checkCollisions();
    void checkDotCollection();
    void nextStage();
    void resetPositions();
    void loseLife();

    GameState state;
    int score;
    int lives;
    int currentStage;
    int stageClearTimer;

    Player player;
    Enemy enemies[NUM_ENEMIES];
    Map gameMap;
};

// Global game instance
extern Game g_game;

#endif // GAME_H
