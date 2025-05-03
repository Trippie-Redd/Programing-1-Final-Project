#include "Raycast.h"
#include "RendererManager.h"
#include <algorithm>

using namespace Primitives2D;

// ------------- Constructors/Destructor ------------- //

Raycast::Raycast() = default;

Raycast::~Raycast() = default;

// ----------------- Update/Render ------------------- //

void Raycast::Render(bool drawHits, uint8_t r, uint8_t g, uint8_t b, uint8_t a) const
{
    // Render each ray as a line from origin to intersection point
    for (const LineSegment& ray : m_rays)
        ray.Render(r, g, b, a);

    if (!drawHits) return;

    // Draw ray hit points
    SDL_Renderer* renderer = RendererManager::GetInstance().GetRenderer();
    SDL_SetRenderDrawColor(renderer, 200, 0, 0, 255);
    for (const Vec2& hit : m_rayHits)
        Rect(Vec2(hit.x - 5, hit.y - 5), 10, 10).Render(r, g, b, a);
}

void Raycast::RenderGeometry()
{
    size_t rayCount = m_rays.size();

    // Can't render tris with only 1 ray
    if (rayCount <= 1)
    {
        std::cout << "Cannot render triangles with fewer than 2 rays" << '\n';
        return;
    }

    // Sort rays based upon angle
    std::sort(m_rays.begin(), m_rays.end(), [](LineSegment a, LineSegment b) { return a.angle < b.angle; });

    SDL_Renderer* renderer = RendererManager::GetInstance().GetRenderer();
    for (int i = 0; i < rayCount; i++)
    {
        // Makes sure that the last tri uses the endpoints of first and last ray
        int nextIdx = (i + 1) % rayCount;

        // Because geometry does not wrap around if rays are not in a circle
        if (nextIdx == 0) break;

        // Defines vertices for tri to render
        SDL_Vertex vertices[] = {
            {{m_rays[i].start.x, m_rays[i].start.y},         {1.0f, 1.0f, 0.0f, 0.7f}}, // Origin point
            {{m_rays[i].end.x, m_rays[i].end.y},             {1.0f, 1.0f, 0.0f, 0.7f}}, // Current ray end point
            {{m_rays[nextIdx].end.x, m_rays[nextIdx].end.y}, {1.0f, 1.0f, 0.0f, 0.7f}}, // Next ray end point
        };

        // Renders tris
        if (!SDL_RenderGeometry(renderer, NULL, vertices, 3, NULL, 0))
            std::cout << "Raycast RenderGeometry failed! Error: " << SDL_GetError() << '\n';
    }
}

// ---------------- Class functions ------------------ //

void Raycast::CastRaysAtVertices(const Vec2& origin, const std::vector<Rect>& environment, const Vec2& fovCenter, float fov)
{
    // Free up rays/rayHits to remove rays no longer needed
    ResetRays();

    // Converts angle from degrees to radians
    fov *= m_PI / 180.0f;

    // Small offset for rays left/right of main ray
    const float ANGLE_OFFSET = 0.0001f;
    const LineSegment referenceLine(origin, fovCenter);

    // Gets the angle of point in center of fov
    Vec2 toFovCenter = fovCenter - origin;
    toFovCenter.Normalize();
    float centralAngle = atan2(toFovCenter.y, toFovCenter.x);

    // Casts two rays alongside the edges of the fov
    // Calculation for left ray
    float leftAngle = centralAngle + fov / 2.0f;
    Vec2 leftDir(cos(leftAngle), sin(leftAngle));
    Vec2 leftRayEnd = origin + leftDir * m_RAY_LENGTH;

    // Calculation for right ray
    float rightAngle = centralAngle - fov / 2.0f;
    Vec2 rightDir(cos(rightAngle), sin(rightAngle));
    Vec2 rightRayEnd = origin + rightDir * m_RAY_LENGTH;

    // Casts the rays
    FindClosestIntersection(origin, leftRayEnd, environment, referenceLine);
    FindClosestIntersection(origin, rightRayEnd, environment, referenceLine);

    for (const Rect& rect : environment)
    {
        // Gets all the corners of the rect
        const Vec2 corners[4] = {
            rect.GetTopLeft(),
            rect.GetTopRight(),
            rect.GetBottomLeft(),
            rect.GetBottomRight()
        };

        // Casts to every rect corner
        for (const Vec2& vertex : corners)
        {
            // Check if vertex is in fov, continue if it is not
            Vec2 toVertexDir = vertex - origin;
            float angleToVertex = atan2(toVertexDir.y, toVertexDir.x);
            float angleDiff = angleToVertex - centralAngle;
            angleDiff = std::remainder(angleDiff, 2.0f * m_PI);
            if (std::abs(angleDiff) > fov / 2.0f) continue;

            // Cast the main ray
            FindClosestIntersection(origin, vertex, environment, referenceLine);

            // Calculate direction vector from origin to vertex
            Vec2 direction = vertex - origin;
            direction.Normalize();

            // Create two rotated direction vectors (left and right offsets)
            // Left offset - rotate counterclockwise
            Vec2 leftDirection;
            leftDirection.x = direction.x * cos(ANGLE_OFFSET) - direction.y * sin(ANGLE_OFFSET);
            leftDirection.y = direction.x * sin(ANGLE_OFFSET) + direction.y * cos(ANGLE_OFFSET);
            leftDirection.Normalize();

            // Right offset - rotate clockwise
            Vec2 rightDirection;
            rightDirection.x = direction.x * cos(-ANGLE_OFFSET) - direction.y * sin(-ANGLE_OFFSET);
            rightDirection.y = direction.x * sin(-ANGLE_OFFSET) + direction.y * cos(-ANGLE_OFFSET);
            rightDirection.Normalize();

            // Calculate endpoints for the offset rays
            Vec2 leftRayEnd = origin + leftDirection * m_RAY_LENGTH;
            Vec2 rightRayEnd = origin + rightDirection * m_RAY_LENGTH;

            // Cast the offset rays
            FindClosestIntersection(origin, leftRayEnd, environment, referenceLine);
            FindClosestIntersection(origin, rightRayEnd, environment, referenceLine);
        }
    }
}

bool Raycast::CastRayToPos(const Vec2& origin, const Vec2& pos, const std::vector<Rect>& environment, bool infiniteLength)
{
    // Casts a ray to a postion, ray is infinitely long
    if (infiniteLength)
    {
        Vec2 direction = pos - origin;
        direction.Normalize();
        Vec2 rayEnd = origin + direction * m_RAY_LENGTH;
        return FindClosestIntersection(origin, rayEnd, environment, LineSegment(origin, origin));
    }
    // Casts a ray to a postion, ray stops at pos
    else
    {
        return FindClosestIntersection(origin, pos, environment, LineSegment(origin, origin));
    }
}

bool Raycast::FindClosestIntersection(const Vec2& origin, const Vec2& rayEnd, const std::vector<Rect>& environment, const LineSegment& referenceLine)
{
    Vec2 closestHit = rayEnd;
    float closestDistance = m_RAY_LENGTH;
    bool result = false;

    for (const Rect& rect : environment)
    {
        // Checks the line segment for intersection with ray
        Intersect intersection = CheckLineRectCollision(
            LineSegment(origin, rayEnd), rect
        );

        // Skips if line segment does not intersect
        if (!intersection.result) continue;

        float distance = (intersection.pos - origin).Length();

        // Checks if current intersection is the closest one
        if (distance < closestDistance)
        {
            closestDistance = distance;
            closestHit = intersection.pos;
        }

        result = true;
    }

    // Adds a ray that stops at the pos of the closest intersection
    LineSegment ray(origin, closestHit);

    SetRayAngle(ray, referenceLine);
    m_rays.push_back(ray);
    m_rayHits.push_back(closestHit);

    return result;
}

void Raycast::SetRayAngle(LineSegment& ray, const LineSegment& referenceLine)
{
    // Calculate rays line vector
    const Vec2 rayVec = ray.end - ray.start;

    // Calculate reference line's vector
    const Vec2 refVec = referenceLine.end - referenceLine.start;

    // Calculate ray angle in relation to ref line segment
    double rAngle = std::atan2(
        Vec2::Cross(rayVec, refVec),
        Vec2::Dot(rayVec, refVec)
    );

    ray.angle = rAngle;
}

void Raycast::ResetRays()
{
    m_rays.clear();
    m_rayHits.clear();
}