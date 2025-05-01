#pragma once

#include "Primitives2D.h"

namespace GameObjects
{
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

        TransitionBox(const Vec2& position, float width, float height, uint16_t nextLevelID, uint16_t keyID = 0);
    };
}