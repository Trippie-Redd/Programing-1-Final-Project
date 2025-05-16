#pragma once

#include "Shotgun.h"
#include <bitset>

class Game;

enum Direction
{
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
        const std::vector<GameObjects::TransitionBox>& transitionBoxes,
        const std::vector<Primitives2D::Circle>& enemies,
        const Vec2& mousePos,
        double deltaTime);
    void Render() const;

    Vec2 GetOrigin()               const { return m_position; }
    float GetHitboxRadius()        const { return m_hitboxRadius; }
    const Shotgun& GetShotgunRef() const { return m_shotgun; }

    void SetCurrentSpeed(float speed) { m_currentSpeed = speed; }
    void SetGamePointer(Game* pGame) { m_pGame = pGame; }

    void Move(enum Direction dir, double deltaTime);
    void Shoot(const std::vector<Primitives2D::Rect>& environment, const Vec2& mousePos);
    void Reload();

public:
    const float WALKING_SPEED = 30.0f;
    const float SPRINTING_SPEED = 60.0f;

private:
    Vec2 m_position;
    float m_hitboxRadius = 10.0f;
    std::vector<Primitives2D::LineSegment> m_body;
    Shotgun m_shotgun;
    bool m_isDead = false;
    Game* m_pGame = nullptr;

    // Movement vars
    float m_currentSpeed = WALKING_SPEED;
    bool m_isSprinting = false;
    Vec2 m_velocity = Vec2::Zero();

    // Cursor vars
    float m_cursorMinRadius = 15.0f;
    float m_cursorMaxRadius = 85.0f;
    float m_cursorCurrentRadius = m_cursorMinRadius;
    Vec2 m_mousePos;

private:
    void UpdateCursor(const Vec2& mousePos);
    float Lerp(float a, float b, float t) const { return a + t * (b - a); }

    void CheckForWallCollisions(const std::vector<Primitives2D::Rect>& environment);
    void CheckForAmmoPickups(std::vector<GameObjects::AmmoCrate>& ammoCrates);
    void CheckForKeyPickups(std::vector<GameObjects::Key>& keys);
	void CheckForTransitionCollisions(const std::vector<GameObjects::TransitionBox>& transitionBoxes);
    void CheckForEnemyCollisions(const std::vector<Primitives2D::Circle>& enemies);

    void UnlockGameObject(GameObjects::GameObjectsEnum type, uint16_t ID);

};