#include "GameObjects.h"

using namespace Primitives2D;

namespace GameObjects
{
    AmmoCrate::AmmoCrate(const Vec2& position, uint8_t ammoCount, uint16_t ID)
        : Rect(position, 30, 20)
        , ammoCount(ammoCount)
        , ID(ID)
    {}

    Key::Key(const Vec2& position, uint16_t ID)
        : Rect(position, 15, 15)
        , ID(ID)
    {}

    Door::Door(const Vec2& position, float width, float height, const Vec2& hingePos, uint16_t ID)
        : Rect(position, width, height)
        , hingePos(hingePos)
        , ID(ID)
    {}

    TransitionBox::TransitionBox(const Vec2& position, float width, float height, uint16_t nextLevelID, uint16_t keyID)
        : Rect(position, width, height)
        , nextLevelID(nextLevelID)
        , keyID(keyID)
    {}
}