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
find_package(SDL3_ttf CONFIG QUIET)
find_package(SDL3_mixer CONFIG QUIET)

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

# If SDL3_mixer is not found, download it
if(NOT SDL3_mixer_FOUND)
    message(STATUS "SDL3_mixer not found, downloading...")
    FetchContent_Declare(
        SDL3_mixer
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_mixer.git
        GIT_TAG main
    )
    # Disable optional features to reduce build time
    set(SDL3MIXER_SAMPLES OFF CACHE BOOL "Build the SDL3_mixer sample programs")
    set(SDL3MIXER_TESTS OFF CACHE BOOL "Build the SDL3_mixer test programs")
    set(SDL3MIXER_OPUS OFF CACHE BOOL "Enable Opus support")
    set(SDL3MIXER_FLAC OFF CACHE BOOL "Enable FLAC support")
    set(SDL3MIXER_MOD OFF CACHE BOOL "Enable MOD support")
    set(SDL3MIXER_MIDI OFF CACHE BOOL "Enable MIDI support")
    # Keep these on as they're commonly used
    set(SDL3MIXER_MP3 ON CACHE BOOL "Enable MP3 support")
    set(SDL3MIXER_OGG ON CACHE BOOL "Enable OGG support")
    FetchContent_MakeAvailable(SDL3_mixer)
endif()

# Add RapidJSON (header-only)
message(STATUS "Adding RapidJSON...")
FetchContent_Declare(
    rapidjson
    GIT_REPOSITORY https://github.com/Tencent/rapidjson.git
    GIT_TAG master
)
FetchContent_MakeAvailable(rapidjson)

# Collect source files
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.h")

# Add the executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Set up library linking
if(TARGET SDL3::SDL3)
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3)
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3-static)
endif()

if(TARGET SDL3_ttf::SDL3_ttf)
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL3_ttf::SDL3_ttf)
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL3_ttf)
endif()

if(TARGET SDL3_mixer::SDL3_mixer)
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL3_mixer::SDL3_mixer)
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL3_mixer)
endif()

# Add RapidJSON headers to include path (header-only library)
target_include_directories(${PROJECT_NAME}
    PRIVATE ${rapidjson_SOURCE_DIR}/include
)

# Add assets and levels directories to the binary directory
file(COPY ${CMAKE_SOURCE_DIR}/levels DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})

# Platform-specific settings
if(WIN32)
    # Copy SDL3 DLLs to output directory on Windows
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:SDL3::SDL3>
        $<TARGET_FILE:SDL3_ttf::SDL3_ttf>
        $<TARGET_FILE:SDL3_mixer::SDL3_mixer>
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )
endif()

# Set debugging and optimization flags
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${PROJECT_NAME} PRIVATE DEBUG)
endif()

# Ensure mixer is properly initialized
target_compile_definitions(${PROJECT_NAME} PRIVATE HAVE_SDL_MIXER)
