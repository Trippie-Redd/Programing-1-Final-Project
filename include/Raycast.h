#pragma once

#include "GameObjects.h"

class Raycast
{
public:
    Raycast()  = default;
    ~Raycast() = default;

    void Render(bool drawHits = false, uint8_t r = 0, uint8_t g = 0, uint8_t b = 0, uint8_t a = 255) const;
    void RenderGeometry() const;
    void SortRays();

    const std::vector<Primitives2D::LineSegment>& GetRays() const { return m_rays; }

    void CastRaysAtVertices(const Vec2& origin,
        const std::vector<Primitives2D::Rect>& environment,
        const Vec2& fovCenter = Vec2::Zero(),
        float fov = 0);
    bool CastRayToPos(const Vec2& origin, const Vec2& pos, const std::vector<Primitives2D::Rect>& environment, bool infiniteLength = false);
    void ResetRays();

private:
    static constexpr float m_RAY_LENGTH = 100000.0f;

    std::vector<Primitives2D::LineSegment> m_rays;
    std::vector<Vec2> m_rayHits;

private:
    bool FindClosestIntersection(const Vec2& origin, const Vec2& rayEnd, const std::vector<Primitives2D::Rect>& environment, const Primitives2D::LineSegment& referenceLine);
    void SetRayAngle(Primitives2D::LineSegment& ray, const Primitives2D::LineSegment& referenceLine);
};