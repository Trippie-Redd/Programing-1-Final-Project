#include "RendererManager.h"

#include <stdexcept>
#include <string>

// Define the static instance
RendererManager* RendererManager::instance = nullptr;

RendererManager& RendererManager::GetInstance()
{
    if (instance == nullptr)
    {
        instance = new RendererManager();
    }
    return *instance;
}

void RendererManager::Destroy()
{
    if (instance != nullptr)
    {
        delete instance;
        instance = nullptr;
    }
}

void RendererManager::Init(SDL_Window* window)
{
    if (m_renderer != nullptr)
    {
        SDL_DestroyRenderer(m_renderer);
    }

    m_renderer = SDL_CreateRenderer(window, nullptr);

    if (m_renderer == nullptr)
    {
        throw std::runtime_error("Failed to create renderer: " + std::string(SDL_GetError()));
    }

    SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);
}

SDL_Renderer* RendererManager::GetRenderer()
{
    if (m_renderer == nullptr)
    {
        throw std::runtime_error("Renderer not initialized");
    }

    return m_renderer;
}

RendererManager::~RendererManager()
{
    if (m_renderer != nullptr)
    {
        SDL_DestroyRenderer(m_renderer);
        m_renderer = nullptr;
    }
}