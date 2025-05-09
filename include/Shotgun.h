#pragma once

#include "Raycast.h"
#include "Text.h"

struct ShotgunBlast
{
    Raycast rays;
    float alpha;

    ShotgunBlast(const Raycast& rays, float alpha)
        : rays(rays), alpha(alpha)
    {
    }
};

class Shotgun
{
public:
    Shotgun();
    ~Shotgun();

    void Update(float deltaTime);
    void Render() const;

    int GetCurrentReserveAmmo() const { return m_currentReserveAmmo; }
    int GetMaxReserveAmmo()     const { return m_maxReserveAmmo; }
    int GetCurrentMagAmmo()     const { return m_currentMagAmmo; }
    int GetMaxMagAmmo()         const { return m_maxMagAmmo; }

    void SetCurrentReserveAmmo(uint8_t value) { m_currentReserveAmmo = value; }
    void SetCurrentMagAmmo(uint8_t value)     { m_currentMagAmmo = value; }

    void AddReserveAmmo(int amount) { m_currentReserveAmmo = m_currentReserveAmmo + amount > m_maxReserveAmmo ? m_maxReserveAmmo : m_currentReserveAmmo + amount; }
    void ClearTraces() { m_blasts.clear(); }

    float Shoot(const std::vector<Primitives2D::Rect>& environment, const Vec2& playerPos, const Vec2& position, float radius, float currentNoise = 0.0f);
    void Reload();

private:
    std::vector<ShotgunBlast> m_blasts;
    Text m_ammoText;
    Text m_testText;

    float m_currentTransparency = 255;

    const int m_bulletAmount = 5;
    int m_maxMagAmmo = 8;
    int m_maxReserveAmmo = 80;
    int m_currentMagAmmo = 0;
    int m_currentReserveAmmo = 0;
};