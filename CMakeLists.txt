cmake_minimum_required(VERSION 3.16)
project(metroid89 C CXX)

set(CMAKE_C_STANDARD 11)

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

target_include_directories(metroid89 PRIVATE port port/platform Src)

# The game type-puns long*/short*/char* over the framebuffer and relies on
# wrapping arithmetic; relax strict aliasing/overflow. Quiet legacy warnings.
target_compile_options(metroid89 PRIVATE
    -fno-strict-aliasing
    -fwrapv
    -Wno-multichar
    -Wno-deprecated-non-prototype
    -Wno-parentheses
    -Wno-pointer-sign
    # Newer clang (Emscripten) makes these hard errors; the game type-puns freely
    # and passes string literals to non-const params. Keep them warnings.
    -Wno-error=incompatible-pointer-types
    -Wno-error=incompatible-function-pointer-types
)

# ---------------------------------------------------------------------------
# Native map data: build the headless MapEditor converter with the HOST C++
# compiler (never the target compiler -- it's a build tool) and run it on the
# .mmp project to emit native-endian map blobs the game loads at runtime.
# ---------------------------------------------------------------------------
set(MAPCONV_DIR ${CMAKE_SOURCE_DIR}/Tool/MapEditor)
set(MAPCONV_SRC
    ${MAPCONV_DIR}/mapconv/mapconv.cpp
    ${MAPCONV_DIR}/map.cpp ${MAPCONV_DIR}/screen.cpp ${MAPCONV_DIR}/enemy.cpp
    ${MAPCONV_DIR}/door.cpp ${MAPCONV_DIR}/Item.cpp ${MAPCONV_DIR}/Pipe.cpp
    ${MAPCONV_DIR}/tiles.cpp ${MAPCONV_DIR}/Background.cpp)
add_custom_command(
    OUTPUT ${CMAKE_SOURCE_DIR}/mtlevel.raw ${CMAKE_SOURCE_DIR}/mtlevel1.raw
    COMMAND c++ -std=gnu++98 -DMAPCONV_NATIVE -w
            -I${MAPCONV_DIR}/mapconv -I${MAPCONV_DIR} ${MAPCONV_SRC}
            -o ${CMAKE_BINARY_DIR}/mapconv
    COMMAND ${CMAKE_BINARY_DIR}/mapconv ${CMAKE_SOURCE_DIR}/Data/timdemo2.mmp ${CMAKE_SOURCE_DIR}
    DEPENDS ${MAPCONV_SRC} ${CMAKE_SOURCE_DIR}/Data/timdemo2.mmp
    COMMENT "Generating native map data (mapconv)"
    VERBATIM)
add_custom_target(mapdata DEPENDS ${CMAKE_SOURCE_DIR}/mtlevel.raw)
add_dependencies(metroid89 mapdata)

if(EMSCRIPTEN)
    # --- WebAssembly build (emcmake cmake ...) ---
    # SDL2 comes from Emscripten's ports. The game has a blocking main loop and
    # busy-wait delays. Rather than proxy it to a worker (PROXY_TO_PTHREAD, which
    # conflicts with the SDL/OffscreenCanvas GL path and is flaky across
    # browsers), we run single-threaded on the browser main thread and compile
    # with ASYNCIFY: every busy-wait yields via platform_yield()/emscripten_sleep
    # so the browser can repaint and deliver input, and the game clock is driven
    # off the wall clock (see port/main_sdl.c). No threads => no SharedArrayBuffer
    # => no COOP/COEP requirement when serving. Data files are baked into the
    # virtual FS under /data (matching DATA_DIR).
    set(DATA_FILES gfx.89z enemy.89z samus.89z tiles.89z mtlevel.raw mtlevel1.raw)
    set(PRELOAD_ARGS "")
    foreach(f ${DATA_FILES})
        # Wrap each pair in SHELL: so CMake keeps the flag+value together and does
        # not de-duplicate the repeated --preload-file flags.
        list(APPEND PRELOAD_ARGS "SHELL:--preload-file ${CMAKE_SOURCE_DIR}/${f}@/data/${f}")
    endforeach()

    target_compile_definitions(metroid89 PRIVATE DATA_DIR="/data")
    target_compile_options(metroid89 PRIVATE -sUSE_SDL=2 -O2)
    target_link_options(metroid89 PRIVATE
        -sUSE_SDL=2
        -O2
        -sASYNCIFY
        -sINITIAL_MEMORY=64MB
        -sEXIT_RUNTIME=1
        -sALLOW_MEMORY_GROWTH=1
        # Persistent save games: IDBFS mounted at /saves (see port/wasm_pre.js).
        -lidbfs.js
        "SHELL:--pre-js ${CMAKE_SOURCE_DIR}/port/wasm_pre.js"
        # Custom page shell: canvas + on-screen touch controls (nipplejs joystick
        # and buttons that drive touch_set_keys() in port/main_sdl.c).
        "SHELL:--shell-file ${CMAKE_SOURCE_DIR}/port/shell.html"
        ${PRELOAD_ARGS})
    # Relink when the pre-js / shell change.
    set_property(TARGET metroid89 APPEND PROPERTY LINK_DEPENDS
        ${CMAKE_SOURCE_DIR}/port/wasm_pre.js
        ${CMAKE_SOURCE_DIR}/port/shell.html)
    # Emit directly into docs/ as index.{html,js,wasm,data} so the repo's docs/
    # folder is a ready-to-publish GitHub Pages site (Settings -> Pages -> docs).
    # emcc writes the .js/.wasm/.data sidecars next to the .html output.
    file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/docs)
    file(TOUCH ${CMAKE_SOURCE_DIR}/docs/.nojekyll)   # serve files verbatim
    set_target_properties(metroid89 PROPERTIES
        OUTPUT_NAME "index"
        SUFFIX ".html"
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/docs)
else()
    # --- Native build ---
    find_package(SDL2 REQUIRED)
    target_include_directories(metroid89 PRIVATE ${SDL2_INCLUDE_DIRS})
    target_link_libraries(metroid89 PRIVATE ${SDL2_LIBRARIES})
    target_compile_definitions(metroid89 PRIVATE DATA_DIR="${CMAKE_SOURCE_DIR}")
endif()
