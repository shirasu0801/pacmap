cmake_minimum_required(VERSION 3.10)
project(pacmap)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files
set(SOURCES
    src/game.cpp
    src/player.cpp
    src/enemy.cpp
    src/map.cpp
    src/bindings.cpp
)

# Add executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Emscripten-specific settings
if(EMSCRIPTEN)
    set(CMAKE_EXECUTABLE_SUFFIX ".js")

    # Emscripten compile and link flags
    set_target_properties(${PROJECT_NAME} PROPERTIES
        LINK_FLAGS "-s WASM=1 \
                    -s EXPORTED_RUNTIME_METHODS=['ccall','cwrap'] \
                    -s ALLOW_MEMORY_GROWTH=1 \
                    -s MODULARIZE=1 \
                    -s EXPORT_NAME='createModule' \
                    -s ENVIRONMENT='web' \
                    -O2"
    )

    # Copy output to web folder
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.js
            ${CMAKE_SOURCE_DIR}/web/${PROJECT_NAME}.js
        COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.wasm
            ${CMAKE_SOURCE_DIR}/web/${PROJECT_NAME}.wasm
        COMMENT "Copying WASM files to web folder"
    )
endif()

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src)
