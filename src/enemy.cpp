#include "enemy.h"
#include "map.h"
#include "player.h"
#include <cstdlib>
#include <cmath>
#include <queue>

Enemy::Enemy() : x(0), y(0), prevX(0), prevY(0), tileX(0), tileY(0),
                 targetTileX(0), targetTileY(0),
                 startX(0), startY(0), direction(0),
                 moveSpeed(0.1f), state(EnemyState::WANDER),
                 needNewDirection(true), allowedToChase(false),
                 eatenTimer(0), recoveryTimer(0), gameMap(nullptr) {
}

void Enemy::init(int sx, int sy, float speed, const Map* mapRef) {
    startX = sx;
    startY = sy;
    tileX = sx;
    tileY = sy;
    x = static_cast<float>(sx);
    y = static_cast<float>(sy);
    prevX = x;
    prevY = y;
    targetTileX = sx;
    targetTileY = sy;
    direction = rand() % 4;
    moveSpeed = speed;
    state = EnemyState::WANDER;
    needNewDirection = true;
    allowedToChase = false;
    eatenTimer = 0;
    recoveryTimer = 60;  // 初期スポーン保護
    gameMap = mapRef;
}

void Enemy::update(const Map& map, const Player& player) {
    // Save previous position for validation
    prevX = x;
    prevY = y;

    // Decrement recovery timer
    if (recoveryTimer > 0) {
        recoveryTimer--;
    }

    // Handle eaten state timer - return to normal after duration
    if (state == EnemyState::EATEN) {
        eatenTimer--;
        if (eatenTimer <= 0) {
            state = EnemyState::WANDER;
            recoveryTimer = RECOVERY_DURATION;  // 復活後1秒間は当たり判定なし
            needNewDirection = true;
        }
    }
    // Check if should chase or wander (not when frightened or eaten)
    // Only chase if this enemy is allowed to chase (closest to player)
    else if (state != EnemyState::FRIGHTENED) {
        int distance = getDistanceToPlayer(player);
        if (allowedToChase && distance < CHASE_THRESHOLD) {
            state = EnemyState::CHASE;
        } else {
            state = EnemyState::WANDER;
        }
    }

    // Check if at tile center
    float distToTileX = std::abs(x - static_cast<float>(tileX));
    float distToTileY = std::abs(y - static_cast<float>(tileY));
    bool atTileCenter = (distToTileX < 0.15f && distToTileY < 0.15f);

    // Choose new direction at tile center
    if (atTileCenter && needNewDirection) {
        chooseNextDirection(map, player);
        needNewDirection = false;
    }

    // Move towards target
    moveTowardsTarget(map);

    // Check if reached new tile
    int newTileX = static_cast<int>(std::round(x));
    int newTileY = static_cast<int>(std::round(y));

    // Handle horizontal wrapping
    newTileX = map.wrapX(newTileX);

    // Clamp Y to map bounds
    if (newTileY < 0) newTileY = 0;
    if (newTileY >= map.getHeight()) newTileY = map.getHeight() - 1;

    if (newTileX != tileX || newTileY != tileY) {
        float distToNewX = std::abs(x - static_cast<float>(newTileX));
        float distToNewY = std::abs(y - static_cast<float>(newTileY));

        // Handle wrap-around distance
        float mapW = static_cast<float>(map.getWidth());
        if (distToNewX > mapW / 2.0f) {
            distToNewX = mapW - distToNewX;
        }

        if (distToNewX < 0.3f && distToNewY < 0.3f) {
            tileX = newTileX;
            tileY = newTileY;
            needNewDirection = true;
        }
    }

    // Validate position - if jumped more than 1 tile, revert to previous
    float jumpX = std::abs(x - prevX);
    float jumpY = std::abs(y - prevY);
    if (jumpX > 1.0f || jumpY > 1.0f) {
        x = prevX;
        y = prevY;
    }
}

int Enemy::findBestDirectionToTarget(const Map& map, int targetX, int targetY, bool isEatenState) {
    int dirs[4][2] = {{0, -1}, {1, 0}, {0, 1}, {-1, 0}}; // UP, RIGHT, DOWN, LEFT
    int opposite[4] = {2, 3, 0, 1};

    int bestDir = -1;
    int bestDist = 9999;

    for (int i = 0; i < 4; i++) {
        // Don't reverse unless necessary
        if (!isEatenState && i == opposite[direction]) {
            continue;
        }

        int nx = tileX + dirs[i][0];
        int ny = tileY + dirs[i][1];

        // Handle wrapping
        nx = map.wrapX(nx);
        ny = map.wrapY(ny);

        if (map.isWalkableForEnemy(nx, ny, isEatenState)) {
            int dist = std::abs(nx - targetX) + std::abs(ny - targetY);
            if (dist < bestDist) {
                bestDist = dist;
                bestDir = i;
            }
        }
    }

    // If no direction found, allow reversing
    if (bestDir < 0) {
        int nx = tileX + dirs[opposite[direction]][0];
        int ny = tileY + dirs[opposite[direction]][1];
        nx = map.wrapX(nx);
        ny = map.wrapY(ny);
        if (map.isWalkableForEnemy(nx, ny, isEatenState)) {
            bestDir = opposite[direction];
        }
    }

    return bestDir;
}

void Enemy::chooseNextDirection(const Map& map, const Player& player) {
    int dirs[4][2] = {{0, -1}, {1, 0}, {0, 1}, {-1, 0}}; // UP, RIGHT, DOWN, LEFT
    int opposite[4] = {2, 3, 0, 1};

    int bestDir = -1;

    if (state == EnemyState::EATEN) {
        // Flee from player - choose direction that maximizes distance (like FRIGHTENED)
        int bestDist = -1;
        for (int i = 0; i < 4; i++) {
            // Allow reversing when eaten to escape faster
            int nx = tileX + dirs[i][0];
            int ny = tileY + dirs[i][1];
            nx = map.wrapX(nx);
            ny = map.wrapY(ny);

            if (map.isWalkableForEnemy(nx, ny, false)) {
                int dist = std::abs(nx - player.getTileX()) + std::abs(ny - player.getTileY());
                if (dist > bestDist) {
                    bestDist = dist;
                    bestDir = i;
                }
            }
        }
    }
    else if (state == EnemyState::CHASE) {
        // Chase player
        bestDir = findBestDirectionToTarget(map, player.getTileX(), player.getTileY(), false);
    }
    else if (state == EnemyState::FRIGHTENED) {
        // Flee from player - choose direction that maximizes distance
        int bestDist = -1;
        for (int i = 0; i < 4; i++) {
            if (i == opposite[direction]) continue;

            int nx = tileX + dirs[i][0];
            int ny = tileY + dirs[i][1];
            nx = map.wrapX(nx);
            ny = map.wrapY(ny);

            if (map.isWalkableForEnemy(nx, ny, false)) {
                int dist = std::abs(nx - player.getTileX()) + std::abs(ny - player.getTileY());
                if (dist > bestDist) {
                    bestDist = dist;
                    bestDir = i;
                }
            }
        }
        // If no good direction, allow reversing
        if (bestDir < 0) {
            int nx = tileX + dirs[opposite[direction]][0];
            int ny = tileY + dirs[opposite[direction]][1];
            nx = map.wrapX(nx);
            ny = map.wrapY(ny);
            if (map.isWalkableForEnemy(nx, ny, false)) {
                bestDir = opposite[direction];
            }
        }
    }
    else {
        // WANDER - random but valid direction
        int validDirs[4];
        int validCount = 0;

        for (int i = 0; i < 4; i++) {
            if (i == opposite[direction]) continue;

            int nx = tileX + dirs[i][0];
            int ny = tileY + dirs[i][1];
            nx = map.wrapX(nx);
            ny = map.wrapY(ny);

            if (map.isWalkableForEnemy(nx, ny, false)) {
                validDirs[validCount++] = i;
            }
        }

        if (validCount > 0) {
            bestDir = validDirs[rand() % validCount];
        } else {
            // Dead end, reverse
            int nx = tileX + dirs[opposite[direction]][0];
            int ny = tileY + dirs[opposite[direction]][1];
            nx = map.wrapX(nx);
            ny = map.wrapY(ny);
            if (map.isWalkableForEnemy(nx, ny, false)) {
                bestDir = opposite[direction];
            }
        }
    }

    if (bestDir >= 0) {
        direction = bestDir;
        targetTileX = tileX + dirs[direction][0];
        targetTileY = tileY + dirs[direction][1];

        // Handle wrapping
        targetTileX = map.wrapX(targetTileX);
        targetTileY = map.wrapY(targetTileY);
    }
}

void Enemy::moveTowardsTarget(const Map& map) {
    float speed = moveSpeed;

    // Eaten enemies move faster to flee
    if (state == EnemyState::EATEN) {
        speed *= 1.5f;
    }
    // Frightened enemies move slower
    else if (state == EnemyState::FRIGHTENED) {
        speed *= 0.6f;
    }

    float targetX = static_cast<float>(targetTileX);
    float targetY = static_cast<float>(targetTileY);
    float mapW = static_cast<float>(map.getWidth());

    // Handle warp tunnel movement
    float dx = targetX - x;
    float dy = targetY - y;

    // Check if we're crossing the warp tunnel
    if (std::abs(dx) > mapW / 2.0f) {
        // Crossing warp tunnel - adjust direction
        if (dx > 0) {
            dx = dx - mapW;  // Go left to wrap
        } else {
            dx = dx + mapW;  // Go right to wrap
        }
    }

    float dist = std::sqrt(dx * dx + dy * dy);

    if (dist > 0.01f) {
        // Move towards target
        if (dist <= speed) {
            x = targetX;
            y = targetY;
        } else {
            x += (dx / dist) * speed;
            y += (dy / dist) * speed;
        }

        // Handle warp tunnel wrapping
        if (x < -0.5f) {
            x += mapW;
            tileX = map.getWidth() - 1;
        } else if (x >= mapW - 0.5f && targetTileX == 0) {
            x -= mapW;
            tileX = 0;
        }

        // Clamp Y to map bounds
        float mapH = static_cast<float>(map.getHeight());
        if (y < 0) y = 0;
        if (y >= mapH) y = mapH - 0.01f;
    }
}

void Enemy::setFrightened(bool frightened) {
    if (frightened && state != EnemyState::EATEN) {
        if (state != EnemyState::FRIGHTENED) {
            // Reverse direction when becoming frightened
            int opposite[4] = {2, 3, 0, 1};
            direction = opposite[direction];
            needNewDirection = true;
        }
        state = EnemyState::FRIGHTENED;
    } else if (!frightened && state == EnemyState::FRIGHTENED) {
        state = EnemyState::WANDER;
    }
}

void Enemy::setCanChase(bool canChase) {
    allowedToChase = canChase;
}

void Enemy::eat() {
    state = EnemyState::EATEN;
    eatenTimer = EATEN_DURATION;  // 3秒後に通常状態に戻る
    needNewDirection = true;
}

int Enemy::getDistanceToPlayer(const Player& player) const {
    return std::abs(tileX - player.getTileX()) + std::abs(tileY - player.getTileY());
}
