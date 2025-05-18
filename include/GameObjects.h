#pragma once

#include "Primitives2D.h"
#include <SDL3/SDL.h>

namespace GameObjects
{
    enum class GameObjectsEnum
    {
        AmmoCrates = 0,
        Keys,
        Enemies,
        GAME_OBJECTS_COUNT = 3
    };

    struct CompleteTexture
    {
        SDL_Texture* texture = nullptr;
        Vec2 dimensions;
    };
    
    // Texture related functions
    void LoadTextures();
    void LoadSingleTexture(const char* filepath, GameObjectsEnum type);
    void DestroyTextures();
    void RenderTexture(const Primitives2D::Rect& object, GameObjectsEnum type);

    // Holds all textures used for game objects
    extern CompleteTexture s_textures[static_cast<int>(GameObjectsEnum::GAME_OBJECTS_COUNT)];
    
    struct AmmoCrate : Primitives2D::Rect
    {
        uint8_t ammoCount;
        uint16_t ID;

        AmmoCrate(const Vec2& position, uint8_t ammoCount, uint16_t ID);
    };

    struct Key : Primitives2D::Rect
    {
        uint16_t ID;

        Key(const Vec2& position, uint16_t ID);
    };

    struct TransitionBox : Primitives2D::Rect
    {
        uint16_t nextLevelID;
        uint16_t keyID;

        TransitionBox(const Vec2& position, float width, float height, uint16_t nextLevelID, uint16_t keyID = 0);
    };
}