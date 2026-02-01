#include "game.h"
#include <cstdlib>
#include <ctime>

Game g_game;

Game::Game() : state(GameState::TITLE), score(0), lives(3),
               currentStage(1), stageClearTimer(0) {
    srand(static_cast<unsigned>(time(nullptr)));
}

void Game::init() {
    state = GameState::TITLE;
    score = 0;
    lives = 3;
    currentStage = 1;
    stageClearTimer = 0;
}

void Game::startGame() {
    state = GameState::PLAYING;
    score = 0;
    lives = 3;
    currentStage = 1;

    gameMap.loadStage(currentStage);
    resetPositions();
}

void Game::resetPositions() {
    player.init(gameMap.getPlayerStartX(), gameMap.getPlayerStartY());

    // Speed varies by stage (higher = faster)
    float baseSpeed = 0.05f + (currentStage - 1) * 0.01f;

    for (int i = 0; i < NUM_ENEMIES; i++) {
        // Each enemy has slightly different speed
        float speedVariation = (rand() % 20 - 10) * 0.002f;
        enemies[i].init(
            gameMap.getEnemyStartX(i),
            gameMap.getEnemyStartY(i),
            baseSpeed + speedVariation,
            &gameMap  // Pass map reference for pathfinding
        );
    }
}

void Game::update() {
    if (state == GameState::TITLE) {
        return;
    }

    if (state == GameState::STAGE_CLEAR) {
        stageClearTimer--;
        if (stageClearTimer <= 0) {
            nextStage();
        }
        return;
    }

    if (state == GameState::GAME_OVER || state == GameState::GAME_CLEAR) {
        return;
    }

    // Update player
    player.update(gameMap);

    // Find the closest enemy to the player (only this one can chase)
    int closestEnemyIndex = -1;
    int closestDistance = 9999;
    for (int i = 0; i < NUM_ENEMIES; i++) {
        if (!enemies[i].isEaten()) {
            int dist = std::abs(enemies[i].getTileX() - player.getTileX()) +
                       std::abs(enemies[i].getTileY() - player.getTileY());
            if (dist < closestDistance) {
                closestDistance = dist;
                closestEnemyIndex = i;
            }
        }
    }

    // Update enemies - only closest can chase
    bool isPower = player.isPowerMode();
    for (int i = 0; i < NUM_ENEMIES; i++) {
        enemies[i].setFrightened(isPower);
        enemies[i].setCanChase(i == closestEnemyIndex);  // 最も近い敵だけが追跡可能
        enemies[i].update(gameMap, player);
    }

    // Check dot collection
    checkDotCollection();

    // Check collisions with enemies
    checkCollisions();

    // Check if stage is clear
    if (gameMap.getDotsRemaining() <= 0) {
        state = GameState::STAGE_CLEAR;
        stageClearTimer = 120; // 2 seconds at 60fps
    }
}

void Game::setInput(int direction) {
    // Always set direction (state check is done in update)
    player.setDirection(static_cast<Direction>(direction));
}

void Game::checkDotCollection() {
    int px = player.getTileX();
    int py = player.getTileY();

    TileType tile = gameMap.getTile(px, py);

    if (tile == TileType::DOT) {
        score += 10;
        gameMap.clearTile(px, py);
    } else if (tile == TileType::POWER_PELLET) {
        score += 50;
        gameMap.clearTile(px, py);
        player.activatePowerMode();

        // Make all enemies frightened
        for (int i = 0; i < NUM_ENEMIES; i++) {
            enemies[i].setFrightened(true);
        }
    }
}

void Game::checkCollisions() {
    // Skip collision check if player is invincible (spawn protection)
    if (player.isInvincible()) {
        return;
    }

    // Use float positions for more accurate collision detection
    float px = player.getX();
    float py = player.getY();

    // Collision threshold - characters must be very close to collide
    // 0.3 means centers must be within 0.3 tiles (about 6 pixels) of each other
    const float COLLISION_THRESHOLD = 0.3f;

    for (int i = 0; i < NUM_ENEMIES; i++) {
        if (enemies[i].isEaten() || enemies[i].isRecovering()) {
            // Already eaten or recovering, ignore
            continue;
        }

        float ex = enemies[i].getX();
        float ey = enemies[i].getY();

        // Skip if enemy position seems invalid (at origin or out of bounds)
        if ((ex < 0.5f && ey < 0.5f) || ex < 0 || ey < 0 ||
            ex >= gameMap.getWidth() || ey >= gameMap.getHeight()) {
            continue;
        }

        // Calculate raw distance between player and enemy (no wrap-around)
        // This prevents "phantom" collisions through tunnel walls
        float dx = px - ex;
        float dy = py - ey;
        float distanceSquared = dx * dx + dy * dy;

        if (distanceSquared < COLLISION_THRESHOLD * COLLISION_THRESHOLD) {
            if (player.isPowerMode()) {
                // Eat the enemy
                enemies[i].eat();
                score += 200;
            } else {
                // Player loses a life
                loseLife();
                return;
            }
        }
    }
}

void Game::loseLife() {
    lives--;
    if (lives <= 0) {
        state = GameState::GAME_OVER;
    } else {
        resetPositions();
    }
}

void Game::nextStage() {
    currentStage++;
    if (currentStage > 3) {
        state = GameState::GAME_CLEAR;
    } else {
        state = GameState::PLAYING;
        gameMap.loadStage(currentStage);
        resetPositions();
    }
}

float Game::getPlayerX() const {
    return player.getX();
}

float Game::getPlayerY() const {
    return player.getY();
}

int Game::getPlayerTileX() const {
    return player.getTileX();
}

int Game::getPlayerTileY() const {
    return player.getTileY();
}

int Game::getPlayerDir() const {
    return player.getDirection();
}

bool Game::isPowerMode() const {
    return player.isPowerMode();
}

bool Game::isPlayerInvincible() const {
    return player.isInvincible();
}

int Game::getPowerTimer() const {
    return player.getPowerTimer();
}

float Game::getEnemyX(int index) const {
    if (index >= 0 && index < NUM_ENEMIES) {
        return enemies[index].getX();
    }
    return 0;
}

float Game::getEnemyY(int index) const {
    if (index >= 0 && index < NUM_ENEMIES) {
        return enemies[index].getY();
    }
    return 0;
}

int Game::getEnemyState(int index) const {
    if (index >= 0 && index < NUM_ENEMIES) {
        return enemies[index].getState();
    }
    return 0;
}

bool Game::isEnemyRecovering(int index) const {
    if (index >= 0 && index < NUM_ENEMIES) {
        return enemies[index].isRecovering();
    }
    return false;
}

int Game::getTile(int x, int y) const {
    return static_cast<int>(gameMap.getTile(x, y));
}

int Game::getMapWidth() const {
    return gameMap.getWidth();
}

int Game::getMapHeight() const {
    return gameMap.getHeight();
}
