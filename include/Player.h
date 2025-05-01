#pragma once

#include "Shotgun.h"

enum Direction {
    UP,
    DOWN,
    LEFT,
    RIGHT
};

class Player
{
public:
    Player();
    ~Player();

    void Update(const std::vector<Primitives2D::Rect>& environment,
        std::vector<GameObjects::AmmoCrate>& ammoCrates,
        std::vector<GameObjects::Key>& keys,
        const Vec2& mousePos,
        double deltaTime);
    void Render() const;

    void setCurrentSpeed(float speed) { m_currentSpeed = speed; };

    Vec2 GetOrigin()        const { return m_position; }
    float GetHitboxRadius() const { return m_hitboxRadius; }
    float GetNoise()        const { return m_noise; }

    void SetUnlockedObjects(bool* pUnlockedGameObjects) { m_pUnlockedGameObjects = pUnlockedGameObjects; }

    void Move(enum Direction dir, double deltaTime);
    void Shoot(const std::vector<Primitives2D::Rect>& environment, const Vec2& mousePos);
    void Reload();

public:
    const float WALKING_SPEED = 30.0f;
    const float SPRINTING_SPEED = 60.0f;

private:
    Vec2 m_position;
    float m_hitboxRadius = 10.0f;
    float m_noise = 0.0f;
    std::vector<Primitives2D::LineSegment> m_body;
    Cursor m_cursor;
    Shotgun m_shotgun;

    // Movement vars
    float m_currentSpeed = WALKING_SPEED;
    bool m_isSprinting = false;
    Vec2 m_velocity = Vec2::Zero();

    // GameObjects to remove in m_unlockedGameObjects in Game class
    bool* m_pUnlockedGameObjects = nullptr;

private:
    void HandleWallCollisions(const std::vector<Primitives2D::Rect>& environment);
    void CheckForAmmoPickups(std::vector<GameObjects::AmmoCrate>& ammoCrates);
    void CheckForKeyPickups(std::vector<GameObjects::Key>& keys);

    void UnlockGameObject(int type, uint16_t ID);

};