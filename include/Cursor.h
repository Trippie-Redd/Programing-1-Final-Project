#pragma once

#include "GameObjects.h"

class Cursor
{
public:
    Cursor(const Vec2& mousePos);
    Cursor();
    ~Cursor();

    void Update(const Vec2& mousePos, const Vec2& playerPos);
    void Render() const;

    float GetRadius() const { return m_currentRadius; }

private:
    Vec2 m_mousePos;
    Vec2 m_playerPos;

    std::vector<Primitives2D::LineSegment> m_centerShape;
    float m_centerShapeMinRadius = 15.0f;
    float m_centerShapeMaxRadius = 85.0f;
    float m_currentRadius;

private:
    float Lerp(float a, float b, float t) const { return a + t * (b - a); }
};