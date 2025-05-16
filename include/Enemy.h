#pragma once

#include "Player.h"
#include <unordered_map>

enum class EnemyStates
{
    Idle,
    Normal,
    Investigating,
    Chasing,
    Deactivated
};

enum class EnemyTypes
{
    Fast,
    Brute,
    Boss,
    Tutorial
};

// Forward declaration of game
class Game;

class Enemy
{
public:
    Enemy(EnemyTypes type, const Primitives2D::LineSegment& path, uint16_t ID, Game* pGame);
    ~Enemy() = default;

    const Primitives2D::Circle& GetHitbox() const { return m_hitbox; }

    void Update(float deltaTime, 
        const Player& player, 
        const std::vector<Primitives2D::Rect>& environment,
        const Shotgun& playerShotgun);
    void Render();

public:
    static const std::unordered_map<std::string, EnemyTypes> enemyMap;
    bool isDead;

private:
    Primitives2D::Circle m_hitbox;
    Raycast m_sight;
    Vec2 m_velocity;
    Vec2 m_position;
    Vec2 m_playerDetectedPosition;
    Vec2 m_targetPosition;
    Primitives2D::LineSegment m_path;
    EnemyStates m_currentState;
    EnemyStates m_lastState;
    Game* m_pGame;

    float m_walkingSpeed;
    float m_chasingSpeed;
    float m_currentSpeed;
    float m_fov = 60.0f;
    float m_idleTimer;
    float m_idleTime;
    uint16_t m_ID;
    int m_health;

private:
    bool CheckIfSeesPlayer(const Player& player, const std::vector<Primitives2D::Rect>& environment);

    void FollowPath(float deltaTime, float speed);
    void Idle(float deltaTime);
    void ChasePlayer(float deltaTime, const Vec2& playerPos);

    bool hasReachedTarget() const { return (m_position - m_targetPosition).Length() <= 2.0f; }
    void SetStats(float hitboxRadius, float walkingSpeed, float chasingSpeed, int health, float idleTime, float fov);
};
