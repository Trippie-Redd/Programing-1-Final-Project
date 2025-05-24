#include "Enemy.h"
#include "Shotgun.h"
#include "Game.h"
#include <bitset>

using namespace Primitives2D;

//-----------------------------------------------------------------------------
// Constructor, enemy stats are decided by the EnemyTypes passed,
// also defines the enemy path and the unique enemy ID
//-----------------------------------------------------------------------------
Enemy::Enemy(EnemyTypes type, const LineSegment& path, uint16_t ID, Game* pGame)
    : m_position(path.start)
    , m_path(path)
    , m_targetPosition(path.end)
    , isDead(false)
    , m_pGame(pGame)
    , m_ID(ID)
{
    switch (type)
    {
    case EnemyTypes::Fast:
        SetStats(20.0f, 50.0f, 70.0f, 5, 2.5f, 60.0f);
        m_currentState = EnemyStates::Normal;
        break;
    case EnemyTypes::Brute:
        SetStats(25.0f, 35.0f, 40.0f, 20, 1.5f, 70.0f);
        m_currentState = EnemyStates::Normal;
        break;
    case EnemyTypes::Boss:
        SetStats(22.0f, 45.0f, 60.0f, 10, 1.0f, 70.0f);
        m_currentState = EnemyStates::Normal;
        break;
    case EnemyTypes::Tutorial:
        SetStats(22.0f, 0.0f, 0.0f, 10, 0.0f, 0.0f);
        m_currentState = EnemyStates::Deactivated;
        break;
    default:
        std::cerr << "Invalid enemy type!" << '\n';
        SetStats(10.0f, 40.0f, 50.0f, 15, 2.0f, 60.0f);
        break;
    }
}


//-----------------------------------------------------------------------------
// Checks for shotgun ray collisions, runs state machine for idle,
// chasing and normal, updates sight raycast and enemy position
//-----------------------------------------------------------------------------
void Enemy::Update(float deltaTime, const Player& player, const std::vector<Rect>& environment, const Shotgun& playerShotgun)
{
    // Checks for collisions with shotgun rays
    const std::vector<ShotgunBlast>& blasts = playerShotgun.GetShotgunBlastsRef();
    for (const ShotgunBlast& blast : blasts)
    {
        const std::vector<LineSegment>& lines = blast.collisionRays.GetRays();
        for (const LineSegment& line : lines)
        {
            if (!CheckLineCircleCollision(line, m_hitbox).result) continue;
            m_health--;
            m_targetPosition = player.GetOrigin();
        }
    }

    // No need to continue updating if enemy is already dead
    if (m_health <= 0)
    {
        m_pGame->GetUnlockedObjects().set(static_cast<int>(GameObjects::GameObjectsEnum::Enemies) * 65536 + m_ID);
        isDead = true;
        return;
    }

    // Used for tutorial enemies, do nothing
    if (m_currentState == EnemyStates::Deactivated) return;

    m_sight.ResetRays();
    m_lastState = m_currentState;

    bool canSeePlayer = CheckIfSeesPlayer(player, environment);
    bool hasReachedTarget = HasReachedTarget();

    // State machine with clearer logic than before
    if (canSeePlayer)
    {
        m_currentState = EnemyStates::Chasing;
        ChasePlayer(deltaTime, player.GetOrigin());
    }
    else if (m_currentState == EnemyStates::Idle)
    {
        // Continue idling until timer expires
        Idle(deltaTime);

        // Exit idle when timer is done
        if (m_idleTimer <= 0.0f)
        {
            m_currentState = EnemyStates::Normal;
            // Timer will be reset next time we enter idle
        }
    }
    else if (hasReachedTarget)
    {
        // Enter idle state
        m_currentState = EnemyStates::Idle;
        Idle(deltaTime);
    }
    else
    {
        // Normal patrolling
        m_currentState = EnemyStates::Normal;
        FollowPath(deltaTime, m_walkingSpeed);
    }

    // Used for visualising sight/fov
    m_sight.CastRaysAtVertices(m_position, environment, m_targetPosition, m_fov);
    m_sight.SortRays();

    // Applies velocity to positon
    m_position += m_velocity * deltaTime;
    m_hitbox.center = m_position;
    m_velocity *= 0.95f;
}


//-----------------------------------------------------------------------------
// Renders sight/fov and body
//-----------------------------------------------------------------------------
void Enemy::Render() const
{
    // Used for tutorial enemies, don't render sight
    if (m_currentState != EnemyStates::Deactivated)
    {
        m_sight.RenderGeometry();
    }
    
    // Renders enemy body
    const std::vector<LineSegment> shape = CreateUniformShape(m_position, static_cast<int>(m_hitbox.radius), 8);
    for (const LineSegment& line : shape)
    {
        line.Render(255, 0, 0, 255);
    }
}


//-----------------------------------------------------------------------------
// Checks if player is visible to enemy
//-----------------------------------------------------------------------------
bool Enemy::CheckIfSeesPlayer(const Player& player, const std::vector<Rect>& environment)
{
    // Get the two points at the borders of circle radius
    const Vec2 playerPos = player.GetOrigin();

    const Vec2 toPlayer = playerPos - m_position;

    // Avoid division by zero
    if (toPlayer.LengthSquared() < 0.001f) return false;

    // Calculate direction to player
    Vec2 toPlayerDir = toPlayer;
    toPlayerDir.Normalize();

    // Calculate the direction of the enemy's FOV center
    Vec2 fovDirection = m_targetPosition - m_position;
    fovDirection.Normalize();

    // Calculate angle between fov direction and direction to player
    float angleInRadians = acos(fovDirection.Dot(toPlayerDir));
    float angleInDegrees = angleInRadians * (180.0f / PI);

    // Check if player is outside FOV angle
    if (angleInDegrees > m_fov / 2.0f) return false;

    // Now do the ray casting checks for the visible player points
    const LineSegment enemyToPlayer{ m_position, playerPos };
    const LineSegment playerLine = enemyToPlayer.CreateCrossingLineSegment(playerPos, player.GetHitboxRadius() * 2);

    Vec2 points[3] = {
        playerPos,
        playerLine.start,
        playerLine.end
    };

    // Cast rays to player points
    for (int i = 0; i < 3; i++)
    {
        // Check if ray hit player without obstacle
        if (!m_sight.CastRayToPos(m_position, points[i], environment)) return true;
    }

    return false;
}


//-----------------------------------------------------------------------------
// Enemy walks in a straight line, from it's current position to it's 
// target position
//-----------------------------------------------------------------------------
void Enemy::FollowPath(float deltaTime, float speed)
{
    const Vec2 direction = m_targetPosition - m_position;

    if (direction.Length() > 0.001f)
        m_velocity = direction.Normalized() * speed * 7.5f;
}


//-----------------------------------------------------------------------------
// Enemy stands completely still for m_idleTime seconds
//-----------------------------------------------------------------------------
void Enemy::Idle(float deltaTime)
{
    m_velocity = Vec2::Zero(); // Always stop while idling

    // If we just entered idle state, reset the timer
    if (m_lastState != EnemyStates::Idle)
    {
        m_idleTimer = m_idleTime;
    }

    // Always decrement timer while idling
    m_idleTimer -= deltaTime;

    // When timer expires, switch target
    if (m_idleTimer <= 0.0f)
    {
        m_targetPosition = (m_targetPosition == m_path.end) ? m_path.start : m_path.end;
    }
}


//-----------------------------------------------------------------------------
// Sets player as target position and m_chasingSpeed as currentspeed
//-----------------------------------------------------------------------------
void Enemy::ChasePlayer(float deltaTime, const Vec2& playerPos)
{
    m_targetPosition = playerPos;
    if (!HasReachedTarget())
        FollowPath(deltaTime, m_chasingSpeed);
}


//-----------------------------------------------------------------------------
// Function to more easily set enemy stats in constructor
//-----------------------------------------------------------------------------
void Enemy::SetStats(float hitboxRadius, 
                     float walkingSpeed,  
                     float chasingSpeed, 
                     int health, 
                     float idleTime,
                     float fov)
{
    m_hitbox           = { m_position, hitboxRadius};
    m_walkingSpeed     = walkingSpeed;
    m_chasingSpeed     = chasingSpeed;
    m_health           = health;
    m_idleTime         = idleTime;
    m_idleTimer        = idleTime;
    m_fov              = fov;
}


//-----------------------------------------------------------------------------
// Maps strings to EnemyTypes, used for clearer labeling in level json files 
//-----------------------------------------------------------------------------
const std::unordered_map<std::string, EnemyTypes> Enemy::enemyMap = {
    { "Fast",      EnemyTypes::Fast     },
    { "Brute",     EnemyTypes::Brute    },
    { "Boss",      EnemyTypes::Boss     },
    { "Tutorial",  EnemyTypes::Tutorial }
};