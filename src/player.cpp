#include "player.h"
#include "map.h"
#include <cmath>

Player::Player() : x(0), y(0), tileX(0), tileY(0),
                   currentDir(Direction::NONE), nextDir(Direction::NONE),
                   powerTimer(0), spawnProtection(0) {
}

void Player::init(int startX, int startY) {
    tileX = startX;
    tileY = startY;
    x = static_cast<float>(startX);
    y = static_cast<float>(startY);
    currentDir = Direction::NONE;
    nextDir = Direction::NONE;
    powerTimer = 0;
    spawnProtection = SPAWN_PROTECTION_TIME;
}

// Helper: check if a direction is walkable from current tile
bool Player::canMove(const Map& map, Direction dir) const {
    int checkX = tileX, checkY = tileY;
    switch (dir) {
        case Direction::UP:    checkY--; break;
        case Direction::DOWN:  checkY++; break;
        case Direction::LEFT:  checkX--; break;
        case Direction::RIGHT: checkX++; break;
        default: return false;
    }
    // Handle horizontal wrapping for warp tunnels
    checkX = map.wrapX(checkX);
    return map.isWalkable(checkX, checkY);
}

void Player::update(const Map& map) {
    // Decrease timers
    if (powerTimer > 0) powerTimer--;
    if (spawnProtection > 0) spawnProtection--;

    // Check if at tile center (threshold must be smaller than MOVE_SPEED)
    float distX = std::abs(x - static_cast<float>(tileX));
    float distY = std::abs(y - static_cast<float>(tileY));
    bool atCenter = (distX < 0.05f && distY < 0.05f);

    // At tile center: try to change direction or continue
    if (atCenter) {
        // Snap to exact center
        x = static_cast<float>(tileX);
        y = static_cast<float>(tileY);

        // Try nextDir first (buffered input)
        if (nextDir != Direction::NONE && canMove(map, nextDir)) {
            currentDir = nextDir;
        }
        // If current direction is blocked, stop
        else if (currentDir != Direction::NONE && !canMove(map, currentDir)) {
            currentDir = Direction::NONE;
        }
    }

    // Move in current direction
    if (currentDir == Direction::NONE) {
        return;
    }

    // Calculate movement
    float moveX = 0, moveY = 0;
    int targetTileX = tileX, targetTileY = tileY;

    switch (currentDir) {
        case Direction::UP:
            moveY = -MOVE_SPEED;
            targetTileY = tileY - 1;
            break;
        case Direction::DOWN:
            moveY = MOVE_SPEED;
            targetTileY = tileY + 1;
            break;
        case Direction::LEFT:
            moveX = -MOVE_SPEED;
            targetTileX = tileX - 1;
            break;
        case Direction::RIGHT:
            moveX = MOVE_SPEED;
            targetTileX = tileX + 1;
            break;
        default:
            return;
    }

    // Handle horizontal wrapping for warp tunnels
    int wrappedTargetX = map.wrapX(targetTileX);

    // Check if target tile is walkable (use wrapped coordinate)
    if (!map.isWalkable(wrappedTargetX, targetTileY)) {
        // Wall ahead - stay at tile center
        x = static_cast<float>(tileX);
        y = static_cast<float>(tileY);
        currentDir = Direction::NONE;
        return;
    }

    // Move towards target tile
    x += moveX;
    y += moveY;

    // Handle warp tunnel (teleport when reaching edge)
    int mapWidth = map.getWidth();
    if (x < -0.5f) {
        // Warping from left to right
        x = static_cast<float>(mapWidth) - 0.5f;
        tileX = mapWidth - 1;
    } else if (x >= static_cast<float>(mapWidth) - 0.5f && targetTileX >= mapWidth) {
        // Warping from right to left
        x = -0.4f;
        tileX = 0;
    }

    // Check if we've reached the next tile (use wrapped coordinate for comparison)
    float targetPosX = static_cast<float>(wrappedTargetX);
    float targetPosY = static_cast<float>(targetTileY);

    float distToTargetX = std::abs(x - targetPosX);
    float distToTargetY = std::abs(y - targetPosY);

    // Handle wrap-around distance calculation
    if (distToTargetX > static_cast<float>(mapWidth) / 2.0f) {
        distToTargetX = static_cast<float>(mapWidth) - distToTargetX;
    }

    if (distToTargetX < 0.05f && distToTargetY < 0.05f) {
        // Arrived at new tile
        tileX = wrappedTargetX;
        tileY = targetTileY;
        x = static_cast<float>(tileX);
        y = static_cast<float>(tileY);
    }
}

void Player::setDirection(Direction dir) {
    nextDir = dir;
    // If not moving, start immediately if possible
    if (currentDir == Direction::NONE) {
        currentDir = dir;
    }
}

void Player::activatePowerMode() {
    powerTimer = POWER_DURATION;
}
