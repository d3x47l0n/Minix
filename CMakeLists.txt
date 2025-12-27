cmake_minimum_required(VERSION 3.20)
project(Minix LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if (MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

include(FetchContent)

# --- raylib ---
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 5.5
)

set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(raylib)

# --- Engine library ---
add_library(Engine STATIC
    Engine/src/Engine.cpp
)

target_include_directories(Engine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Engine/include
)

target_link_libraries(Engine PUBLIC raylib)

# --- Editor ---
add_executable(Editor
    Editor/src/main.cpp
)
target_link_libraries(Editor PRIVATE Engine)

# --- Player ---
add_executable(Player
    Player/src/main.cpp
)
target_link_libraries(Player PRIVATE Engine)

# --- Test ---
add_executable(Test
    Test/src/main.cpp
)
target_link_libraries(Test PRIVATE Engine)
