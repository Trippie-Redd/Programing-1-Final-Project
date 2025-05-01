#include "Enemy.h"

using namespace Primitives2D;

Enemy::Enemy(EnemyTypes type, const Vec2& position, const LineSegment& path, uint16_t ID)
    : m_position(position)
    , m_path(path)
    , m_targetPosition(path.end)
    , m_currentState(EnemyStates::Normal)
    , m_ID(ID)
{
    switch (type)
    {
    case EnemyTypes::Melee:
        break;
    case EnemyTypes::Brute:
        break;
    case EnemyTypes::Pistol:
        break;
    case EnemyTypes::Shotgun:
        break;
    case EnemyTypes::Boss:
        break;
    default:
        std::cout << "Invalid enemy type!" << '\n';
        break;
    }
}

Enemy::~Enemy() = default;

void Enemy::Update(float deltaTime, const Player& player, const std::vector<Primitives2D::Rect>& environment, const std::vector<GameObjects::Door>& doors)
{
    m_sight.ResetRays();

    // Saves last state enemy was in
    m_lastState = m_currentState;

    // Check which state enemy is in
    if (CheckIfSeesPlayer(player, environment)) m_currentState = EnemyStates::Chasing;
    else if (CheckIfHearsPlayer(player))        m_currentState = EnemyStates::Investigating;
    else if (hasReachedTarget())                m_currentState = EnemyStates::Idle;
    else                                        m_currentState = EnemyStates::Normal;

    m_tempSight = m_sight;

    // Calculate FOV to visualize later
    m_sight.CastRaysAtVertices(m_position, environment, m_targetPosition, m_fov);

    // State machine
    switch (m_currentState)
    {
    case EnemyStates::Normal:
        FollowPath(deltaTime, m_walkingSpeed);
        break;
    case EnemyStates::Idle:
        Idle(deltaTime);
        break;
    case EnemyStates::Investigating:
        Investigate(deltaTime, player.GetOrigin());
        break;
    case EnemyStates::Chasing:
        ChasePlayer(deltaTime, player.GetOrigin());
        break;
    default:
        std::cout << "Non-existent state assigned to enemy m_currentState" << '\n';
        break;
    }

    m_position += m_velocity * deltaTime;
    m_velocity *= 0.98f;

    m_shape = Primitives2D::CreateUniformShape(m_position, 10.0f, 8);
}

void Enemy::Render()
{
    m_sight.RenderGeometry();
    m_tempSight.Render(true, 255, 255, 255, 255);

    for (const LineSegment& line : m_shape)
    {
        line.Render(255, 0, 0, 255);
    }
}

bool Enemy::CheckIfSeesPlayer(const Player& player, const std::vector<Primitives2D::Rect>& environment)
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
    float angleInDegrees = angleInRadians * (180.0f / 3.14159f);

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

bool Enemy::CheckIfHearsPlayer(const Player& player) const
{
    const float playerNoise = player.GetNoise();
    const float distance = LineSegment(player.GetOrigin(), m_position).Length();

    return playerNoise >= distance;
}

void Enemy::FollowPath(float deltaTime, float speed)
{
    const Vec2 direction = m_targetPosition - m_position;

    if (direction.Length() > 0.001f)
        m_velocity += direction.Normalized() * speed * deltaTime * 60.0f;
}

void Enemy::Idle(float deltaTime)
{
    m_velocity = Vec2::Zero(); // Always stop while idling

    if (m_lastState == EnemyStates::Idle && m_idleTimer <= 0.0f)
    {
        m_targetPosition = m_targetPosition == m_path.end ? m_path.start : m_path.end;
    }
    else if (m_lastState == EnemyStates::Idle)
    {
        m_idleTimer -= deltaTime;
    }
    else
    {
        m_idleTimer = 3.0f;
    }
}

void Enemy::Investigate(float deltaTime, const Vec2& playerPos)
{
    if (m_lastState != EnemyStates::Investigating)
        m_targetPosition = playerPos;
    else if (!hasReachedTarget())
        FollowPath(deltaTime, m_investigateSpeed);
}

void Enemy::ChasePlayer(float deltaTime, const Vec2& playerPos)
{
    m_targetPosition = playerPos;
    if (!hasReachedTarget())
        FollowPath(deltaTime, m_chasingSpeed);
}

const std::unordered_map<std::string, EnemyTypes> Enemy::enemyMap = {
    { "Normal",  EnemyTypes::Melee   },
    { "Brute",   EnemyTypes::Brute   },
    { "Pistol",  EnemyTypes::Pistol  },
    { "Shotgun", EnemyTypes::Shotgun },
    { "Boss",    EnemyTypes::Boss    }
};
