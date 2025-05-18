#include "Player.h"
#include "Game.h"
#include "Settings.h"
#include "AudioManager.h"
#include <algorithm>

using namespace Primitives2D;

//-----------------------------------------------------------------------------
// Constructor, places player in the middle of the screen
//-----------------------------------------------------------------------------
Player::Player()
    : m_position(Vec2(Settings::WINDOW_WIDTH / 2, Settings::WINDOW_HEIGHT / 2))
{}


//-----------------------------------------------------------------------------
// Applies velocity to position, checks for all player collisions, 
// creates shape for body, updates shotgun and cursor  
//-----------------------------------------------------------------------------
void Player::Update(const std::vector<Rect>& environment,
                    std::vector<GameObjects::AmmoCrate>& ammoCrates,
                    std::vector<GameObjects::Key>& keys, 
                    const std::vector<GameObjects::TransitionBox>& transitionBoxes,
                    const std::vector<Primitives2D::Circle>& enemies,
                    const Vec2& mousePos, 
                    double deltaTime)
{
    // Only thing that needs to be updated when player is dead is enemy collisions
    // Because of sudo-timer
    if (m_isDead)
    {
        CheckForEnemyCollisions(enemies);
        return;
    }
    
    // Apply position and decrease velocity
    m_position += m_velocity * deltaTime;
    m_velocity *= 0.9f;

    // Check for collisions with all game objects
    CheckForWallCollisions(environment);
    CheckForTransitionCollisions(transitionBoxes);
    CheckForAmmoPickups(ammoCrates);
    CheckForKeyPickups(keys);
    CheckForEnemyCollisions(enemies);

    // Creates shape for body, updates shotgun and cursor  
    m_body = CreateUniformShape(m_position, 10.0f, 8);
    m_shotgun.Update(deltaTime);
    UpdateCursor(mousePos);
}


//-----------------------------------------------------------------------------
// Renders body shape, shotgun blasts and cursor shape
//-----------------------------------------------------------------------------
void Player::Render() const
{
    // No need to render anything if player is dead
    if (m_isDead) return;

    // Renders body shape
    for (const LineSegment& line : m_body)
        line.Render(0, 255, 0, 255);

    // Renders shoutgun blasts
    m_shotgun.Render();
   
    // Render cursor center shape
    const std::vector<Primitives2D::LineSegment> centerShape = CreateUniformShape(m_mousePos, m_cursorCurrentRadius, 8);
    for (const LineSegment& segment : centerShape)
        segment.Render(255, 0, 0, 255);
}


//-----------------------------------------------------------------------------
// Creates cursor shape, lerps size based on distance from player position
//-----------------------------------------------------------------------------
void Player::UpdateCursor(const Vec2& mousePos)
{
    m_mousePos = mousePos;

    float length = LineSegment(m_mousePos, m_position).Length();
    float minLength = 30;
    float maxLength = 850;

    // Caps length
    if (length > maxLength)
        length = maxLength;
    else if (length < minLength)
        length = minLength;

    // Calculates cursor radius
    float lerpLength = (length - minLength) / (maxLength - minLength);
    m_cursorCurrentRadius = Lerp(m_cursorMinRadius, m_cursorMaxRadius, lerpLength);
}


//-----------------------------------------------------------------------------
// Handles WASD/arrowkeys input
//-----------------------------------------------------------------------------
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


//-----------------------------------------------------------------------------
// Helper function for Shotgun::Shoot() 
//-----------------------------------------------------------------------------
void Player::Shoot(const std::vector<Rect>& environment, const Vec2& mousePos)
{
    // Shouldn't shoot if player is dead
    if (m_isDead) return;

    m_shotgun.Shoot(environment, m_position, mousePos, m_cursorCurrentRadius);
}


//-----------------------------------------------------------------------------
// Helper function for Shotgun::Reload() 
//-----------------------------------------------------------------------------
void Player::Reload()
{
    // Shouldn't reload if player is dead
    if (m_isDead) return;

    m_shotgun.Reload();
}


//-----------------------------------------------------------------------------
// Checks for wall collisions and applies proper velocity adjustments
//-----------------------------------------------------------------------------
void Player::CheckForWallCollisions(const std::vector<Rect>& environment)
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


//-----------------------------------------------------------------------------
// Checks for transition box collisions and do normal wall collisions if 
// transition box hasn't been unlocked, otherwise transition to new level
//-----------------------------------------------------------------------------
void Player::CheckForTransitionCollisions(const std::vector<GameObjects::TransitionBox>& transitionBoxes)
{
    for (const GameObjects::TransitionBox& box : transitionBoxes)
    {
        // Do nothing if player is not colliding
        if (!CheckRectCircleCollision(box, { m_position, m_hitboxRadius })) continue;

        // Check if the player should transition
        if (m_pGame->GetUnlockedObjects().test(static_cast<int>(GameObjects::GameObjectsEnum::Keys) * 65536 + box.keyID))
        {
            // Puts player in the right position
            // True if transition box is on the bottom or top of the screen
            if ((box.GetTopLeft() - box.GetTopRight()).Length() < (box.GetTopLeft() - box.GetBottomLeft()).Length())
            {
                // True if transition box is on the right
                if (m_position.x > Settings::WINDOW_WIDTH / 2)
                    m_position.x = 50;
                else
                    m_position.x = Settings::WINDOW_WIDTH - 50;
            }
            else
            {
                // True if transition box is on the bottom
                if (m_position.y > Settings::WINDOW_HEIGHT / 2)
                    m_position.y = 50;
                else
                    m_position.y = Settings::WINDOW_HEIGHT - 50;
            }

            // Removes all shotgun traces
            m_shotgun.ClearTraces();

            // Just loads a new level
            m_pGame->LoadLevel(box.nextLevelID);
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


//-----------------------------------------------------------------------------
// Checks for ammo box collisions, player picks up ammo if current reserve 
// ammo is not maxed out
//-----------------------------------------------------------------------------
void Player::CheckForAmmoPickups(std::vector<GameObjects::AmmoCrate>& ammoCrates)
{
    // Don't pickup if ammo is already maxed out
    if (m_shotgun.GetCurrentReserveAmmo() >= m_shotgun.GetMaxReserveAmmo()) return;

    for (size_t i = 0; i < ammoCrates.size(); i++)
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


//-----------------------------------------------------------------------------
// Checks for key collisions, player picks up key upon collision
//-----------------------------------------------------------------------------
void Player::CheckForKeyPickups(std::vector<GameObjects::Key>& keys)
{
    for (size_t i = 0; i < keys.size(); i++)
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


//-----------------------------------------------------------------------------
// Checks for enemy collisions, game restarts upon collision
//-----------------------------------------------------------------------------
void Player::CheckForEnemyCollisions(const std::vector<Primitives2D::Circle>& enemies)
{
    for (const Primitives2D::Circle& enemy : enemies)
    {
        if (!CheckCircleCircleCollision({ m_position, m_hitboxRadius }, enemy)) continue;
        
        // Runs when player is in game over screen
        if (m_isDead)
        {
            // Resets all unlocked game objects
            m_pGame->GetUnlockedObjects().reset();

            // Places player at middle of screen
            m_position = { Settings::WINDOW_WIDTH / 2, Settings::WINDOW_HEIGHT / 2 };
            m_velocity = Vec2::Zero();

            // Starts song again
            AudioManager::GetInstance().Play(AudioEnum::Music);

            m_pGame->LoadLevel(1);
            m_isDead = false;
        }
        // Puts player into game over screen when touching an enemy
        else
        {
            m_isDead = true;

            // Stops all audio and plays game over sfx
            AudioManager::GetInstance().StopAll();
            AudioManager::GetInstance().Play(AudioEnum::GameOver);

            // Resets ammunition and shotgun traces
            m_shotgun.SetCurrentReserveAmmo(0);
            m_shotgun.SetCurrentMagAmmo(0);
            m_shotgun.ClearTraces();

            m_position = { -3000.0f, 0.0f };

            m_pGame->LoadLevel(999); // Game over screen
        }
    }
}


//-----------------------------------------------------------------------------
// Unlocks specified game object in main game instance
//-----------------------------------------------------------------------------
void Player::UnlockGameObject(GameObjects::GameObjectsEnum type, uint16_t ID)
{
    m_pGame->GetUnlockedObjects().set(static_cast<int>(type) * 65536 + ID);
}