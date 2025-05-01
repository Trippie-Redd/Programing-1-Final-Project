#pragma once 

#include "Vec2.h"

#include <iostream>
#include <vector>

namespace Primitives2D
{
    struct Intersect
    {
        bool result;
        Vec2 pos;
    };

    struct LineSegment
    {
        Vec2 start;
        Vec2 end;
        double angle;

        LineSegment() = default;

        LineSegment(const Vec2& start, const Vec2& end)
            : start(start)
            , end(end)
        {}

        LineSegment(const float x1, const float y1, const float x2, const float y2)
            : start(Vec2(x1, y1))
            , end(Vec2(x2, y2))
        {}

        void Render(uint8_t r, uint8_t g, uint8_t b, uint8_t a) const;

        LineSegment CreateCrossingLineSegment(const Vec2& middlePoint, float length) const;
        float Length() const { return std::sqrt(std::pow(end.x - start.x, 2) + std::pow(end.y - start.y, 2)); }
    };

    struct Circle
    {
        Circle() = default;
        Circle(const Vec2& center, const float radius)
            : center(center)
            , radius(radius)
        {}

        Vec2 center;
        float radius;
    };

    struct Rect
    {
        Vec2 min;
        Vec2 max;

        Rect() = default;
        Rect(const Vec2& position, float width, float height)
            : min(position)
            , max(position + Vec2(width, height))
        {}

        void Render(uint8_t r, uint8_t g, uint8_t b, uint8_t a, bool fillRect = true) const;

        Vec2 GetTopLeft()     const { return min; }
        Vec2 GetBottomRight() const { return max; }
        Vec2 GetTopRight()    const { return Vec2(min.x, max.y); }
        Vec2 GetBottomLeft()  const { return Vec2(min.x, max.y); }
        float GetWidth()      const { return max.x - min.x; }
        float GetHeight()     const { return max.y - min.y; }
    };

    // Collision detection functions
    std::vector<LineSegment> CreateUniformShape(const Vec2& pos, float radius, int sides);

    Vec2 ClosestPointOnLine(const Vec2& point, const LineSegment& line);
    Intersect CheckLineCircleCollision(const LineSegment& line, const Circle& circle);
    Intersect CheckLineRectCollision(const LineSegment& line, const Rect& rect);
    Intersect CheckLineLineCollision(const Vec2& p1, const Vec2& p2, const Vec2& q1, const Vec2& q2);

    bool CheckCircleCircleCollision(const Circle& circle1, const Circle& circle2);
    bool CheckRectRectCollision(const Rect& rect1, const Rect& rect2);
    bool CheckRectCircleCollision(const Rect& rect, const Circle& circle);
}