#include "Shotgun.h"

// ------------- Constructors/Destructor ------------- //

Shotgun::Shotgun() = default;

Shotgun::~Shotgun() = default;

// ----------------- Update/Render ------------------- //

void Shotgun::Update(float deltaTime)
{
    for (size_t i = 0; i < m_blasts.size(); i++)
    {
        // Removes shotgun ray collision after 1 frame
        if (m_blasts[i].collisionChecked)
        {
            // Only clear collisionRays if they have not previously been cleared
            if (!m_blasts[i].collisionRays.GetRays().empty())
            {
                m_blasts[i].collisionRays.ResetRays();
            }
        }
        else
        {
            m_blasts[i].collisionChecked = true;
        }

        // Decreases constanly alpha, removes from vector if alpha is < 0
        m_blasts[i].alpha -= 95.0f * deltaTime;

        if (m_blasts[i].alpha <= 0)
        {
            m_blasts.erase(m_blasts.begin() + i);
            i--;
        }
    }

    // Updates ammo count text
    char ammoText[8];
    sprintf(ammoText, "%d%c%d", m_currentMagAmmo, '/', m_currentReserveAmmo);
    m_ammoText.CreateTextTexture(ammoText, strlen(ammoText), 22.0f, { 255, 0, 0 }, Vec2(4.0f, 4.0f));

}

void Shotgun::Render() const
{
    // Render every line for every shotgunblast
    for (const ShotgunBlast& blast : m_blasts)
    {
        if (blast.alpha > 0)
            blast.rays.Render(false, 255, 0, 0, (uint8_t)blast.alpha);
    }

    // Renders the ammo count
    m_ammoText.RenderTexture();
}

// ---------------- Class functions ------------------ //

// Returns a noise value
void Shotgun::Shoot(const std::vector<Primitives2D::Rect>& environment, const Vec2& playerPos, const Vec2& position, float radius)
{
    if (m_currentMagAmmo <= 0) return;
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
}

void Shotgun::Reload()
{
    // First calc ammount of ammo to take from mag
    uint8_t ammoToReload = m_maxMagAmmo - m_currentMagAmmo;
    
    // Do nothing if mag is full or can't reload
    if (ammoToReload <= 0)         return;
    if (m_currentReserveAmmo <= 0) return;

    // If player has less than ammo needed to fully reload
    if (m_currentReserveAmmo < ammoToReload)
    {
        ammoToReload = m_currentReserveAmmo;
    }

    // Change ammo counts
    m_currentReserveAmmo -= ammoToReload;
    m_currentMagAmmo += ammoToReload;
}