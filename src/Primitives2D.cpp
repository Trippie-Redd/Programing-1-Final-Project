#include "Primitives2D.h"

#include "RendererManager.h"

namespace Primitives2D
{
    //-----------------------------------------------------------------------------
    // Renders a line with a specified color and opacity
    //-----------------------------------------------------------------------------
    void LineSegment::Render(uint8_t r, uint8_t g, uint8_t b, uint8_t a) const
    {
        SDL_Renderer* renderer = RendererManager::GetInstance().GetRenderer();
        SDL_SetRenderDrawColor(renderer, r, g, b, a);
        if (!SDL_RenderLine(renderer, start.x, start.y, end.x, end.y))
        {
            std::cout << "SDL_RenderLine in LineSegment failed! Error: " << SDL_GetError() << '\n';
        }
    }


    //-----------------------------------------------------------------------------
    // Renders a rect with a specified color and opacity, can render both 
    // filled and non-filled rects
    //-----------------------------------------------------------------------------
    void Rect::Render(uint8_t r, uint8_t g, uint8_t b, uint8_t a, bool fillRect) const
    {
        SDL_Renderer* renderer = RendererManager::GetInstance().GetRenderer();
        SDL_SetRenderDrawColor(renderer, r, g, b, a);

        const SDL_FRect rect = { min.x, min.y, GetWidth(), GetHeight() };
        // If rectangle should be a solid color
        if (fillRect) 
        {
            if (!SDL_RenderFillRect(renderer, &rect))
            {
                std::cerr << "SDL_RenderFillRect in struct Rect failed! Error: " << SDL_GetError() << '\n';
            }
            return;
        }
        // If rectangle should be rendered hollow in the middle
        if (!SDL_RenderRect(renderer, &rect))
        {
            std::cerr << "SDL_RenderRect in struct Rect failed! Error: " << SDL_GetError() << '\n';
        }
    }


    //-----------------------------------------------------------------------------
    // Returns a vector of {sides} element, shaped in a uniform shape around
    // {pos} with a radius of {radius}
    //-----------------------------------------------------------------------------
    std::vector<LineSegment> CreateUniformShape(const Vec2& pos, float radius, int sides)
    {
        std::vector<LineSegment> segments;

        // Calculate points for a regular shape
        for (int i = 0; i < sides; i++)
        {
            // USE PI!!!
            float angle1 = 2.0f * PI * i / sides;
            float angle2 = 2.0f * PI * (i + 1) / sides;

            Vec2 p1(pos.x + radius * cosf(angle1),
                pos.y + radius * sinf(angle1));
            Vec2 p2(pos.x + radius * cosf(angle2),
                pos.y + radius * sinf(angle2));

            segments.push_back(LineSegment(p1, p2));
        }

        return segments;
    }


    //-----------------------------------------------------------------------------
    // Create a perpendicular line segment that crosses the original one
    // Used for enemy - player detection sight
    //-----------------------------------------------------------------------------
    LineSegment LineSegment::CreateCrossingLineSegment(const Vec2& middlePoint, float length) const
    {
        // Calculate direction vector of original line
        Vec2 d = end - start;

        // Calculate perpendicular direction vector (rotate 90 degrees)
        d.y *= -1.0f;

        // Normalize the perpendicular vector
        d.Normalize();

        // Create the perpendicular line segment with specified length
        LineSegment perpendicular{
            {middlePoint.x - d.x * length / 2, middlePoint.y - d.y * length / 2},
            {middlePoint.x + d.x * length / 2, middlePoint.y + d.y * length / 2}
        };

        return perpendicular;
    }


    //-----------------------------------------------------------------------------
    // Takes in Vec2 point and LineSegment, returns the Vec2 which is closest to
    // point and on the LineSegment
    //-----------------------------------------------------------------------------
    Vec2 ClosestPointOnLine(const Vec2& point, const LineSegment& line)
    {
        Vec2 lineVec = line.end - line.start;
        Vec2 pointVec = point - line.start;

        float lineLength = line.Length();
        float projection = pointVec.Dot(lineVec) / (lineLength * lineLength);

        // Clamp projection to line segment bounds
        projection = std::max(0.0f, std::min(1.0f, projection));

        // Calculate closest point on line
        return line.start + lineVec * projection;
    }


    //-----------------------------------------------------------------------------
    // Takes in LineSegment and Circle, returns true 
    // (aswell as the postition of their first collision) if they collide,
    // otherwise returns false
    //-----------------------------------------------------------------------------
    Intersect CheckLineCircleCollision(const LineSegment& line, const Circle& circle)
    {
        Intersect result;

        // Calculate closest point on line to circle center
        Vec2 closestPoint = ClosestPointOnLine(circle.center, line);

        // Calculate squared distance from closest point to circle center
        float lengthSquared = Vec2(closestPoint.x - circle.center.x, closestPoint.y - circle.center.y).LengthSquared();

        // Check if closest point is within circle radius
        result.result = lengthSquared <= circle.radius * circle.radius;

        // Calculate intersection point (approximation - uses closest point)
        if (result.result)
            result.pos = closestPoint;

        return result;
    }


    //-----------------------------------------------------------------------------
    // Takes in LineSegment and Rect, returns true 
    // (aswell as the postition of their first collision) if they collide,
    // otherwise returns false
    //-----------------------------------------------------------------------------
    Intersect CheckLineRectCollision(const LineSegment& line, const Rect& rect)
    {
        Intersect result;
        result.result = false;

        // Line equation parameters
        float x1 = line.start.x;
        float y1 = line.start.y;
        float x2 = line.end.x;
        float y2 = line.end.y;

        // Rect boundaries
        float xmin = rect.min.x;
        float ymin = rect.min.y;
        float xmax = rect.max.x;
        float ymax = rect.max.y;

        // Check if line is completely outside Rect
        if ((x1 < xmin && x2 < xmin) || (x1 > xmax && x2 > xmax) ||
            (y1 < ymin && y2 < ymin) || (y1 > ymax && y2 > ymax))
            return result;


        // Check if line start or end is inside Rect
        if ((x1 >= xmin && x1 <= xmax && y1 >= ymin && y1 <= ymax) ||
            (x2 >= xmin && x2 <= xmax && y2 >= ymin && y2 <= ymax))
        {
            result.result = true;

            // Return the point that's inside the Rect
            result.pos = (x1 >= xmin && x1 <= xmax && y1 >= ymin && y1 <= ymax) ? line.start : line.end;

            return result;
        }

        // Line parameters for parametric equation: point = start + t * (end - start)
        float dx = x2 - x1;
        float dy = y2 - y1;

        // Calculate intersection with each edge of the Rect
        float tmin = 0.0f;
        float tmax = 1.0f;

        // Check intersection with vertical boundaries
        if (dx != 0.0f)
        {
            float tx1 = (xmin - x1) / dx;
            float tx2 = (xmax - x1) / dx;

            tmin = std::max(tmin, std::min(tx1, tx2));
            tmax = std::min(tmax, std::max(tx1, tx2));
        }

        // Check intersection with horizontal boundaries
        if (dy != 0.0f)
        {
            float ty1 = (ymin - y1) / dy;
            float ty2 = (ymax - y1) / dy;

            tmin = std::max(tmin, std::min(ty1, ty2));
            tmax = std::min(tmax, std::max(ty1, ty2));
        }

        // Check if there is a valid intersection
        if (tmax >= tmin && tmax >= 0.0f && tmin <= 1.0f)
        {
            result.result = true;

            // Calculate intersection point
            float t = tmin > 0.0f ? tmin : tmax;
            result.pos.x = x1 + t * dx;
            result.pos.y = y1 + t * dy;
        }

        return result;
    }


    //-----------------------------------------------------------------------------
    // Takes in two LineSegments, returns true 
    // (aswell as the postition of their first collision) if they collide,
    // otherwise returns false
    //-----------------------------------------------------------------------------
    Intersect CheckLineLineCollision(const Vec2& p1, const Vec2& p2, const Vec2& q1, const Vec2& q2)
    {
        Vec2 r(p2 - p1);
        Vec2 s(q2 - q1);
        Vec2 qp(q1 - p1);

        float rxs = Vec2::Cross(r, s);
        float qpxr = Vec2::Cross(qp, r);
        float qpxs = Vec2::Cross(qp, s);

        // For colinear or parallel lines
        if (std::abs(rxs) < 1e-8)
            return { false, Vec2() };

        float t = qpxs / rxs;
        float u = qpxr / rxs;

        if (t >= 0 && t <= 1 && u >= 0 && u <= 1)
            return { true, p1 + r * t };
        else
            return { false, Vec2() };
    }


    //-----------------------------------------------------------------------------
    // Takes in two Circles, returns true if they collide,
    // otherwise returns false
    //-----------------------------------------------------------------------------
    bool CheckCircleCircleCollision(const Circle& circle1, const Circle& circle2)
    {
        float lengthSquared = Vec2(circle1.center.x - circle2.center.x, circle1.center.y - circle2.center.y).LengthSquared();

        float radiusSum = circle1.radius + circle2.radius;

        return lengthSquared <= radiusSum * radiusSum;
    }


    //-----------------------------------------------------------------------------
    // Takes in two Rects, returns true if they collide, otherwise returns false
    // Unused, might remove
    //-----------------------------------------------------------------------------
    bool CheckRectRectCollision(const Rect& rect1, const Rect& rect2)
    {
        // Check if one Rect is to the left/right of the other
        if (rect1.max.x < rect2.min.x || rect2.max.x < rect1.min.x)
            return false;

        // Check if one Rect is above/below the other
        if (rect1.max.y < rect2.min.y || rect2.max.y < rect1.min.y)
            return false;

        // If neither of the above, Rectes must overlap
        return true;
    }


    //-----------------------------------------------------------------------------
    // Takes in Rect and Circle, returns true if they collide, 
    // otherwise returns false
    //-----------------------------------------------------------------------------
    bool CheckRectCircleCollision(const Rect& rect, const Circle& circle)
    {
        // Find closest point on Rect to circle center
        Vec2 closest;

        // Clamp circle center to Rect
        closest.x = std::max(rect.min.x, std::min(circle.center.x, rect.max.x));
        closest.y = std::max(rect.min.y, std::min(circle.center.y, rect.max.y));

        // Calculate squared distance between closest point and circle center
        float lengthSquared = Vec2(closest.x - circle.center.x, closest.y - circle.center.y).LengthSquared();

        // Check if distance is less than or equal to circle radius
        return lengthSquared <= circle.radius * circle.radius;
    }
}