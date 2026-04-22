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

# --- box2d ---
FetchContent_Declare(
    box2d
    GIT_REPOSITORY https://github.com/erincatto/box2d.git
    GIT_TAG v3.1.0
)

FetchContent_MakeAvailable(raylib box2d)

# --- lua ---
FetchContent_Declare(
    lua_src
    URL https://www.lua.org/ftp/lua-5.4.8.tar.gz
)

FetchContent_Populate(lua_src)

set(LUA_SRC_DIR ${lua_src_SOURCE_DIR}/src)

add_library(lua STATIC
    ${LUA_SRC_DIR}/lapi.c
    ${LUA_SRC_DIR}/lauxlib.c
    ${LUA_SRC_DIR}/lbaselib.c
    ${LUA_SRC_DIR}/lcode.c
    ${LUA_SRC_DIR}/lcorolib.c
    ${LUA_SRC_DIR}/lctype.c
    ${LUA_SRC_DIR}/ldblib.c
    ${LUA_SRC_DIR}/ldebug.c
    ${LUA_SRC_DIR}/ldo.c
    ${LUA_SRC_DIR}/ldump.c
    ${LUA_SRC_DIR}/lfunc.c
    ${LUA_SRC_DIR}/lgc.c
    ${LUA_SRC_DIR}/linit.c
    ${LUA_SRC_DIR}/liolib.c
    ${LUA_SRC_DIR}/llex.c
    ${LUA_SRC_DIR}/lmathlib.c
    ${LUA_SRC_DIR}/lmem.c
    ${LUA_SRC_DIR}/loadlib.c
    ${LUA_SRC_DIR}/lobject.c
    ${LUA_SRC_DIR}/lopcodes.c
    ${LUA_SRC_DIR}/loslib.c
    ${LUA_SRC_DIR}/lparser.c
    ${LUA_SRC_DIR}/lstate.c
    ${LUA_SRC_DIR}/lstring.c
    ${LUA_SRC_DIR}/lstrlib.c
    ${LUA_SRC_DIR}/ltable.c
    ${LUA_SRC_DIR}/ltablib.c
    ${LUA_SRC_DIR}/ltm.c
    ${LUA_SRC_DIR}/lundump.c
    ${LUA_SRC_DIR}/lutf8lib.c
    ${LUA_SRC_DIR}/lvm.c
    ${LUA_SRC_DIR}/lzio.c
)

target_include_directories(lua PUBLIC ${LUA_SRC_DIR})

# --- Engine library ---
add_library(Engine STATIC
    "Engine/src/MinixRuntime.cpp"  
    "Engine/src/App.cpp" 
    "Engine/src/Registry.cpp" 
    "Engine/src/LuaRuntime.cpp" 
    "Engine/src/LuaAPI.cpp" 
    "Engine/src/PhysicsRuntime.cpp" 
    "Engine/src/InputRuntime.cpp" 
    "Engine/src/Camera.cpp" "Engine/src/ComponentPosition.cpp" "Engine/src/ComponentDraw.cpp" "Engine/src/ComponentPhysics.cpp" "Engine/src/Messager.cpp" "Engine/src/SchedulerRuntime.cpp" "Engine/src/EventPhysicsTouched.cpp")

target_include_directories(Engine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Engine/include
)

target_link_libraries(Engine PUBLIC raylib box2d lua)

# --- Editor ---
add_executable(Editor WIN32
    Editor/src/main.cpp)
target_link_libraries(Editor PRIVATE Engine)

# --- Player ---
add_executable(Player WIN32
    Player/src/main.cpp)
target_link_libraries(Player PRIVATE Engine)

# --- Test ---
add_executable(Test
    Test/src/main.cpp)
target_link_libraries(Test PRIVATE Engine)
