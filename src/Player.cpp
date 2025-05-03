#include "Player.h"

#include <algorithm>

using namespace Primitives2D;

// ------------- Constructors/Destructor ------------- //

Player::Player()
    : m_cursor(Cursor(Vec2::Zero()))
    , m_position(Vec2(600, 400))
{}

Player::~Player() = default;

// ----------------- Update/Render ------------------- //

void Player::Update(const std::vector<Rect>& environment,
                    std::vector<GameObjects::AmmoCrate>& ammoCrates,
                    std::vector<GameObjects::Key>& keys, 
                    const std::vector<GameObjects::TransitionBox>& transitionBoxes,
                    const Vec2& mousePos, 
                    double deltaTime)
{
    // Decrease noise value
    m_noise = m_noise > m_hitboxRadius ? m_noise * 0.9f * deltaTime : 0.0f;

    m_position += m_velocity * deltaTime;
    m_velocity *= 0.9f;

    HandleWallCollisions(environment);
    CheckForTransitionCollisions(transitionBoxes);
    CheckForAmmoPickups(ammoCrates);
    CheckForKeyPickups(keys);

    m_body = CreateUniformShape(m_position, 10.0f, 8);
    m_cursor.Update(mousePos, m_position);
    m_shotgun.Update(deltaTime);
}

void Player::Render() const
{
    if (m_noise > m_hitboxRadius)
    {
        std::vector<Primitives2D::LineSegment> noiseVisual = CreateUniformShape(m_position, m_noise, 8);
        for (const LineSegment& line : noiseVisual)
            line.Render(0, 220, 255, 200);
    }

    for (const LineSegment& line : m_body)
        line.Render(0, 255, 0, 255);

    m_shotgun.Render();

    m_cursor.Render();
}

// ---------------- Class functions ------------------ //

void Player::Move(enum Direction dir, double deltaTime)
{
    float speed = m_currentSpeed;// * deltaTime;

    switch (dir)
    {
    case UP:
        m_velocity.y -= speed;
        break;
    case DOWN:
        m_velocity.y += speed;
        break;
    case LEFT:
        m_velocity.x -= speed;
        break;
    case RIGHT:
        m_velocity.x += speed;
        break;
    default:
        std::cout << "Invalid movment direction given to Player::Move()!" << '\n';
        break;
    }
}

void Player::Shoot(const std::vector<Rect>& environment, const Vec2& mousePos)
{
    m_noise = m_shotgun.Shoot(environment, m_position, mousePos, m_cursor.GetRadius(), m_noise);
    //std::cout << m_noise << '\n';
}

void Player::Reload()
{
    m_shotgun.Reload();
}

void Player::HandleWallCollisions(const std::vector<Rect>& environment)
{
    bool collided = false;
    Vec2 totalCorrection(0, 0);

    // Check collisions with all walls
    for (const Rect& wall : environment)
    {
        // Do nothing if player has not collided
        if (!CheckRectCircleCollision(wall, { m_position, m_hitboxRadius })) continue;

        // Find closest point on wall to player center
        Vec2 closest(
            std::clamp(m_position.x, wall.min.x, wall.max.x),
            std::clamp(m_position.y, wall.min.y, wall.max.y)
        );

        // Calculate penetration vector
        Vec2 penetrationVector = m_position - closest;
        float distance = penetrationVector.Length();

        // Normalize and scale by penetration depth
        if (distance > 0)
        {
            float penetrationDepth = m_hitboxRadius - distance;
            if (penetrationDepth > 0)
            {
                totalCorrection += penetrationVector.Normalized() * penetrationDepth;
                collided = true;
            }
        }
    }

    // Do nothing if player has not collided
    if (!collided) return;

    // Apply the correction vector
    m_position += totalCorrection;

    // Calculate surface normal based on correction
    Vec2 surfaceNormal = totalCorrection.Normalized();

    // Calculate and apply slide velocity if moving into the surface
    float dotProduct = Vec2::Dot(m_velocity, surfaceNormal);
    if (dotProduct < 0)
    {
        m_velocity -= surfaceNormal * dotProduct;
        m_velocity *= 0.8f; // Apply friction
    }
}

void Player::CheckForTransitionCollisions(const std::vector<GameObjects::TransitionBox>& transitionBoxes)
{
    for (const GameObjects::TransitionBox& box : transitionBoxes)
    {
        // Do nothing if player is not colliding
        if (!CheckRectCircleCollision(box, { m_position, m_hitboxRadius })) continue;

        // Check if the player should transition
        if (m_pUnlockedGameObjects->test(static_cast<int>(GameObjects::GameObjectsEnum::Keys) * 65536 + box.keyID))
        {
            // Make sure player is in the right position
            m_position = box.nextPos;

            // TODO : Load new level

        }
        else
        {
            // Find closest point on wall to player center
            Vec2 closest(
                std::clamp(m_position.x, box.min.x, box.max.x),
                std::clamp(m_position.y, box.min.y, box.max.y)
            );

            // Calculate penetration vector
            Vec2 penetrationVector = m_position - closest;
            float distance = penetrationVector.Length();

            // Normalize and scale by penetration depth
            Vec2 correction(Vec2::Zero());
            if (distance > 0)
            {
                float penetrationDepth = m_hitboxRadius - distance;
                if (penetrationDepth > 0)
                {
                    correction = penetrationVector.Normalized() * penetrationDepth;
                }
            }

            // Apply the correction vector
            m_position += correction;

            // Calculate surface normal based on correction
            Vec2 surfaceNormal = correction.Normalized();

            // Calculate and apply slide velocity if moving into the surface
            float dotProduct = Vec2::Dot(m_velocity, surfaceNormal);
            if (dotProduct < 0)
            {
                m_velocity -= surfaceNormal * dotProduct;
                m_velocity *= 0.8f; // Apply friction
            }
        }
    }
}

void Player::CheckForAmmoPickups(std::vector<GameObjects::AmmoCrate>& ammoCrates)
{
    // Don't pickup if ammo is already maxed out
    if (m_shotgun.GetCurrentReserveAmmo() >= m_shotgun.GetMaxReserveAmmo()) return;

    for (int i = 0; i < ammoCrates.size(); i++)
    {
        if (CheckRectCircleCollision(Primitives2D::Rect(ammoCrates[i].GetTopLeft(), ammoCrates[i].GetWidth(), ammoCrates[i].GetHeight()), { m_position, m_hitboxRadius }))
        {
            m_shotgun.AddReserveAmmo(ammoCrates[i].ammoCount);

            // Add ammoCrate to m_unlockedGameObjects in Game class
            // Change this, do not pass in 0
            UnlockGameObject(GameObjects::GameObjectsEnum::AmmoCrates, ammoCrates[i].ID);

            // Remove object from list
            ammoCrates.erase(ammoCrates.begin() + i);

            break;
        }
    }
}

void Player::CheckForKeyPickups(std::vector<GameObjects::Key>& keys)
{
    for (int i = 0; i < keys.size(); i++)
    {
        if (CheckRectCircleCollision(Primitives2D::Rect(keys[i].GetTopLeft(), keys[i].GetWidth(), keys[i].GetHeight()), { m_position, m_hitboxRadius }))
        {
            // Add ammoCrate to m_unlockedGameObjects in Game class
            // Change this, do not pass in 1
            UnlockGameObject(GameObjects::GameObjectsEnum::Keys, keys[i].ID);

            // Remove object from list
            keys.erase(keys.begin() + i);

            break;
        }
    }
}

void Player::UnlockGameObject(GameObjects::GameObjectsEnum type, uint16_t ID)
{
    if (m_pUnlockedGameObjects == nullptr)
    {
        std::cout << "m_pUnlockedGameObjects has not been set" << '\n';
        return;
    }

    m_pUnlockedGameObjects->set(static_cast<int>(type) * 65536 + ID);
}