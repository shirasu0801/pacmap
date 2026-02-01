#include "game.h"
#include <emscripten/emscripten.h>

extern "C" {

EMSCRIPTEN_KEEPALIVE
void game_init() {
    g_game.init();
}

EMSCRIPTEN_KEEPALIVE
void game_start() {
    g_game.startGame();
}

EMSCRIPTEN_KEEPALIVE
void game_update() {
    g_game.update();
}

EMSCRIPTEN_KEEPALIVE
void game_set_input(int direction) {
    g_game.setInput(direction);
}

EMSCRIPTEN_KEEPALIVE
int game_get_state() {
    return g_game.getState();
}

EMSCRIPTEN_KEEPALIVE
int game_get_score() {
    return g_game.getScore();
}

EMSCRIPTEN_KEEPALIVE
int game_get_lives() {
    return g_game.getLives();
}

EMSCRIPTEN_KEEPALIVE
int game_get_stage() {
    return g_game.getStage();
}

EMSCRIPTEN_KEEPALIVE
float game_get_player_x() {
    return g_game.getPlayerX();
}

EMSCRIPTEN_KEEPALIVE
float game_get_player_y() {
    return g_game.getPlayerY();
}

EMSCRIPTEN_KEEPALIVE
int game_get_player_dir() {
    return g_game.getPlayerDir();
}

EMSCRIPTEN_KEEPALIVE
int game_is_power_mode() {
    return g_game.isPowerMode() ? 1 : 0;
}

EMSCRIPTEN_KEEPALIVE
int game_is_player_invincible() {
    return g_game.isPlayerInvincible() ? 1 : 0;
}

EMSCRIPTEN_KEEPALIVE
int game_get_power_timer() {
    return g_game.getPowerTimer();
}

EMSCRIPTEN_KEEPALIVE
float game_get_enemy_x(int index) {
    return g_game.getEnemyX(index);
}

EMSCRIPTEN_KEEPALIVE
float game_get_enemy_y(int index) {
    return g_game.getEnemyY(index);
}

EMSCRIPTEN_KEEPALIVE
int game_get_enemy_state(int index) {
    return g_game.getEnemyState(index);
}

EMSCRIPTEN_KEEPALIVE
int game_is_enemy_recovering(int index) {
    return g_game.isEnemyRecovering(index) ? 1 : 0;
}

EMSCRIPTEN_KEEPALIVE
int game_get_tile(int x, int y) {
    return g_game.getTile(x, y);
}

EMSCRIPTEN_KEEPALIVE
int game_get_map_width() {
    return g_game.getMapWidth();
}

EMSCRIPTEN_KEEPALIVE
int game_get_map_height() {
    return g_game.getMapHeight();
}

EMSCRIPTEN_KEEPALIVE
int game_get_num_enemies() {
    return g_game.getNumEnemies();
}

}
