#pragma once

#include "Raycast.h"
#include "Text.h"

struct ShotgunBlast
{
    Raycast rays;
    Raycast collisionRays;
    bool collisionChecked;
    float alpha;

    ShotgunBlast(const Raycast& rays, float alpha)
        : rays(rays)
        , collisionRays(rays)
        , collisionChecked(false)
        , alpha(alpha)
    {
    }
};

class Shotgun
{
public:
    Shotgun()  = default;
    ~Shotgun() = default;

    void Update(float deltaTime);
    void Render() const;

    int GetCurrentReserveAmmo()                            const { return m_currentReserveAmmo; }
    int GetMaxReserveAmmo()                                const { return m_maxReserveAmmo; }
    int GetCurrentMagAmmo()                                const { return m_currentMagAmmo; }
    int GetMaxMagAmmo()                                    const { return m_maxMagAmmo; }
    const std::vector<ShotgunBlast>& GetShotgunBlastsRef() const { return m_blasts; }

    void SetCurrentReserveAmmo(uint8_t value) { m_currentReserveAmmo = value; }
    void SetCurrentMagAmmo(uint8_t value)     { m_currentMagAmmo = value; }

    void AddReserveAmmo(int amount) { m_currentReserveAmmo = m_currentReserveAmmo + amount > m_maxReserveAmmo ? m_maxReserveAmmo : m_currentReserveAmmo + amount; }
    void ClearTraces() { m_blasts.clear(); }

    void Shoot(const std::vector<Primitives2D::Rect>& environment, const Vec2& playerPos, const Vec2& position, float radius);
    void Reload();

private:
    std::vector<ShotgunBlast> m_blasts;
    Text m_ammoText;

    const int m_bulletAmount = 5;
    int m_maxMagAmmo = 8;
    int m_maxReserveAmmo = 80;
    int m_currentMagAmmo = 0;
    int m_currentReserveAmmo = 0;
};