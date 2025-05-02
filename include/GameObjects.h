#pragma once

#include "Primitives2D.h"

namespace GameObjects
{
    static constexpr int GAME_OBJECTS_ENUM_LENGTH = 4;
    enum class GameObjectsEnum
    {
		AmmoCrates,
		Keys,
		Doors,
		Enemies
    };
    
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

    struct Door : Primitives2D::Rect
    {
        Vec2 hingePos;
        uint16_t ID;

        Door(const Vec2& position, float width, float height, const Vec2& hingePos, uint16_t ID);
    };

    struct TransitionBox : Primitives2D::Rect
    {
        uint16_t nextLevelID;
        uint16_t keyID;
        Vec2 nextPos;

        TransitionBox(const Vec2& position, float width, float height, uint16_t nextLevelID, const Vec2& nextPos, uint16_t keyID = 0);
    };
}