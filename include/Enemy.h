#pragma once

#include "Player.h"

#include <unordered_map>

enum class EnemyStates
{
    Idle,
    Normal,
    Investigating,
    Chasing
};

enum class EnemyTypes
{
    Fast,
    Brute,
    Boss
};

class Enemy
{
public:
    Enemy(EnemyTypes type, const Primitives2D::LineSegment& path, uint16_t ID);
    ~Enemy();

    Primitives2D::Circle GetHitbox() const { return m_hitbox; }

    void Update(float deltaTime, const Player& player, const std::vector<Primitives2D::Rect>& environment, const std::vector<GameObjects::Door>& doors);
    void Render();

public:
    static const std::unordered_map<std::string, EnemyTypes> enemyMap;

private:
    Primitives2D::Circle m_hitbox;
    Raycast m_sight;
    Raycast m_tempSight;
    Vec2 m_velocity;
    Vec2 m_position;
    Vec2 m_playerDetectedPosition;
    Vec2 m_targetPosition;
    Primitives2D::LineSegment m_path;
    EnemyStates m_currentState;
    EnemyStates m_lastState;
    std::vector<Primitives2D::LineSegment> m_shape;

    float m_walkingSpeed;
    float m_investigateSpeed;
    float m_chasingSpeed;
    float m_currentSpeed;
    float m_fov = 60.0f;
    float m_idleTimer;
    uint16_t m_ID;
    int m_health;

private:
    bool CheckIfSeesPlayer(const Player& player, const std::vector<Primitives2D::Rect>& environment);
    bool CheckIfHearsPlayer(const Player& player) const;

    void FollowPath(float deltaTime, float speed);
    void Idle(float deltaTime);
    void Investigate(float deltaTime, const Vec2& playerPos);
    void ChasePlayer(float deltaTime, const Vec2& playerPos);

    bool hasReachedTarget() const { return (m_position - m_targetPosition).Length() <= 2.0f; }
    void SetStats(float hitboxRadius, float walkingSpeed, float investigateSpeed, float chasingSpeed, int health, float idleTimer, float fov);
};
