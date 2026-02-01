@echo off
setlocal

echo ========================================
echo   PacMap C++ to WASM Build Script
echo ========================================
echo.

REM Change to project directory
cd /d "%~dp0"

REM Delete old build files first
echo [CLEAN] Deleting old build files...
if exist "web\pacmap.js" del "web\pacmap.js"
if exist "web\pacmap.wasm" del "web\pacmap.wasm"
echo [OK] Old files deleted
echo.

REM Set emcc path directly
set EMCC=C:\emsdk\upstream\emscripten\emcc.bat

REM Verify emcc exists
if not exist "%EMCC%" (
    echo [ERROR] Emscripten not found at %EMCC%
    echo Please install emsdk at C:\emsdk
    pause
    exit /b 1
)

echo [OK] Emscripten found
call "%EMCC%" --version
echo.

echo [BUILD] Compiling C++ to WebAssembly...
echo.

REM Direct emcc compilation with explicit function exports
call "%EMCC%" src/game.cpp src/player.cpp src/enemy.cpp src/map.cpp src/bindings.cpp -o web/pacmap.js -std=c++17 -O2 -sWASM=1 -sEXPORTED_RUNTIME_METHODS=ccall,cwrap -sALLOW_MEMORY_GROWTH=1 -sMODULARIZE=1 -sEXPORT_NAME=createModule -sENVIRONMENT=web -sEXPORTED_FUNCTIONS=_game_init,_game_start,_game_update,_game_set_input,_game_get_state,_game_get_score,_game_get_lives,_game_get_stage,_game_get_player_x,_game_get_player_y,_game_get_player_dir,_game_is_power_mode,_game_is_player_invincible,_game_get_power_timer,_game_get_enemy_x,_game_get_enemy_y,_game_get_enemy_state,_game_is_enemy_recovering,_game_get_tile,_game_get_map_width,_game_get_map_height,_game_get_num_enemies -I src

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo [ERROR] Build failed!
    pause
    exit /b 1
)

echo.
echo ========================================
echo   Build Successful!
echo ========================================
echo.
echo Output files:
dir web\pacmap.* /b 2>nul
echo.
echo Build time: %date% %time%
echo.
echo IMPORTANT: Clear browser cache (Ctrl+Shift+R) after reload!
echo.
echo To run the game:
echo   1. cd web
echo   2. python -m http.server 8080
echo   3. Open http://localhost:8080
echo.
pause
