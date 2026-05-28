cmake_minimum_required(VERSION 3.16)
project(metroid89 C)

set(CMAKE_C_STANDARD 11)

find_package(SDL2 REQUIRED)

# Game sources (the former exe + game/sys "DLL" members), edited in place.
set(GAME_SRC
    Src/metroid.c
    Src/Utility.c
    Src/Player.c
    Src/Map.c
    Src/Shots.c
    Src/Bomb.c
    Src/Enemy.c
    Src/Bar.c
    Src/Door.c
    Src/Blur.c
    Src/Powerup.c
    Src/MiscGFX.c
    Src/dllClipSprites.c
    Src/DLLTestCollide.c
    Src/DLLMap.c
    Src/DLLBackground.c
    Src/DLLBlock.c
    Src/DLLEntity.c
    Src/DLLTiles.c
    Src/DLLAnimation.c
    Src/DLLEnvironment.c
    Src/DLLPipe.c
    Src/EntityTest.c
    Src/DLLSystem.c
    Src/DLLTitle.c
    # metpack is built in memory at startup from these native data tables
    # (was a separate flash file on the calculator).
    Src/Metdata.c
    Src/DLLFrameData.c
    Src/DLLEnemyData.c
    Src/DLLShotData.c
    Src/DLLBackgroundData.c
    # Src/unpack.c -- ttunpack decompressor is m68k asm; ported to C in Phase 1.
)

# Native platform shim + SDL frontend.
set(PORT_SRC
    port/main_sdl.c
    port/platform/stubs.c
    port/platform/tifile.c
    port/platform/ttunpack_port.c
    port/platform/sprite_shim.c
)

add_executable(metroid89 ${GAME_SRC} ${PORT_SRC})

target_include_directories(metroid89 PRIVATE
    port
    port/platform
    Src
    ${SDL2_INCLUDE_DIRS}
)

target_link_libraries(metroid89 PRIVATE ${SDL2_LIBRARIES})

# Where the .89z/.89y data files live (the repo root, for now).
target_compile_definitions(metroid89 PRIVATE DATA_DIR="${CMAKE_SOURCE_DIR}")

# The game type-puns long*/short*/char* over the framebuffer and relies on
# wrapping arithmetic; relax strict aliasing/overflow. Quiet legacy warnings.
target_compile_options(metroid89 PRIVATE
    -fno-strict-aliasing
    -fwrapv
    -Wno-multichar
    -Wno-deprecated-non-prototype
    -Wno-parentheses
    -Wno-pointer-sign
)
