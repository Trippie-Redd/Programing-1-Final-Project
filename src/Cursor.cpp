#include "Cursor.h"

using namespace Primitives2D;

// ------------- Constructors/Destructor ------------- //

Cursor::Cursor() = default;

Cursor::Cursor(const Vec2& mousePos)
    : m_mousePos(mousePos)
{
    m_centerShape = CreateUniformShape(m_mousePos, 5.0f, 8);
}

Cursor::~Cursor() = default;

// ----------------- Update/Render ------------------- //

void Cursor::Update(const Vec2& mousePos, const Vec2& playerPos)
{
    m_mousePos = mousePos;
    m_playerPos = playerPos;

    float length = LineSegment(mousePos, playerPos).Length();
    float minLength = 30;
    float maxLength = 850;

    // Caps length
    if (length > maxLength)
        length = maxLength;
    else if (length < minLength)
        length = minLength;

    // Calculates cursor radius
    float lerpLength = (length - minLength) / (maxLength - minLength);
    m_currentRadius = Lerp(m_centerShapeMinRadius, m_centerShapeMaxRadius, lerpLength);

    m_centerShape = CreateUniformShape(m_mousePos, m_currentRadius, 8);
}

void Cursor::Render() const
{
    // Render center shape
    for (const LineSegment& segment : m_centerShape)
        segment.Render(255, 0, 0, 255);
}