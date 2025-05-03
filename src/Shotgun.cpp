#include "Shotgun.h"

// ------------- Constructors/Destructor ------------- //

Shotgun::Shotgun() = default;

Shotgun::~Shotgun() = default;

// ----------------- Update/Render ------------------- //

void Shotgun::Update(float deltaTime)
{
    // Decreases constanly alpha, removes from vector if alpha is < 0
    for (int i = 0; i < m_blasts.size(); i++)
    {
        m_blasts[i].alpha -= 95.0f * deltaTime;

        // bout to do sum crazy here
        // hol up
        // still havent done anything crazy
        if (m_blasts[i].alpha <= 0)
        {
            m_blasts.erase(m_blasts.begin() + i);
            i--;
        }
    }
}

void Shotgun::Render() const
{
    // Render every line for every shotgunblast
    for (const ShotgunBlast& blast : m_blasts)
    {
        if (blast.alpha > 0)
            blast.rays.Render(false, 255, 0, 0, (uint8_t)blast.alpha);
    }
}

// ---------------- Class functions ------------------ //

// Returns a noise value
float Shotgun::Shoot(const std::vector<Primitives2D::Rect>& environment, const Vec2& playerPos, const Vec2& position, float radius, float currentNoise)
{
    if (m_currentMagAmmo <= 0) return 0;
    m_currentMagAmmo--;

    Raycast newBlast;

    for (int i = 0; i < m_bulletAmount; i++)
    {
        // Generate random radians 0 - 2PI
        float angle = (float)rand() / RAND_MAX * 2.0f * 3.1415927f; // PI

        // Generate random distance 0 - radius
        float distance = sqrt((float)rand() / RAND_MAX) * radius;

        // Calculate bullet target pos
        Vec2 bulletPosition(
            position.x + distance * cos(angle),
            position.y + distance * sin(angle)
        );

        newBlast.CastRayToPos(playerPos, bulletPosition, environment, true);
    }

    m_blasts.emplace_back(newBlast, 255.0f);

    // Calculate noise value 
    currentNoise += 1;
    return 30 * std::pow(currentNoise, 0.3);
}

void Shotgun::Reload()
{
    m_currentMagAmmo = m_maxMagAmmo;
}