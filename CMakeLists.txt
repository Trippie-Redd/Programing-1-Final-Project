cmake_minimum_required(VERSION 3.16)
project(MySDL3App)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directory for header files
include_directories(include)

# Auto-download packages if not found
include(FetchContent)

# Try to find packages normally first
find_package(SDL3 CONFIG QUIET)
find_package(SDL3_image CONFIG QUIET)
find_package(SDL3_ttf CONFIG QUIET)

# If SDL3 is not found, download it
if(NOT SDL3_FOUND)
    message(STATUS "SDL3 not found, downloading...")
    FetchContent_Declare(
        SDL3
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG main
    )
    FetchContent_MakeAvailable(SDL3)
endif()

# If SDL3_image is not found, download it
if(NOT SDL3_image_FOUND)
    message(STATUS "SDL3_image not found, downloading...")
    FetchContent_Declare(
        SDL3_image
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git
        GIT_TAG main
    )
    set(SDL3IMAGE_SAMPLES OFF CACHE BOOL "Build the SDL3_image sample programs")
    set(SDL3IMAGE_TESTS OFF CACHE BOOL "Build the SDL3_image test programs")
    FetchContent_MakeAvailable(SDL3_image)
endif()

# If SDL3_ttf is not found, download it
if(NOT SDL3_ttf_FOUND)
    message(STATUS "SDL3_ttf not found, downloading...")
    FetchContent_Declare(
        SDL3_ttf
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf.git
        GIT_TAG main
    )
    set(SDL3TTF_SAMPLES OFF CACHE BOOL "Build the SDL3_ttf sample programs")
    set(SDL3TTF_TESTS OFF CACHE BOOL "Build the SDL3_ttf test programs")
    FetchContent_MakeAvailable(SDL3_ttf)
endif()

# Collect source files
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.h")

# Add the executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} 
    "include/Vec2.h" "src/Vec2.cpp"
    "include/RendererManager.h" "src/RendererManager.cpp"
    "include/Primitives2D.h" "src/Primitives2D.cpp"
    "include/Text.h" "src/Text.cpp"
    "include/GameObjects.h" "src/GameObjects.cpp"
    "include/Cursor.h" "src/Cursor.cpp"
    "include/Raycast.h" "src/Raycast.cpp"
    "include/Shotgun.h" "src/Shotgun.cpp"
    "include/Player.h" "src/Player.cpp"
    "include/Enemy.h" "src/Enemy.cpp"
    "include/Game.h" "src/Game.cpp"
    "include/Settings.h"
)

# Link libraries
target_link_libraries(${PROJECT_NAME}
    PRIVATE
    SDL3::SDL3
    SDL3_image::SDL3_image
    SDL3_ttf::SDL3_ttf
)