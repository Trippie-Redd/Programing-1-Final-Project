#include "RendererManager.h"

#include <iostream>
#include <string>

// Define the static instance
RendererManager* RendererManager::s_instance = nullptr;

RendererManager& RendererManager::GetInstance()
{
    if (s_instance == nullptr)
    {
        s_instance = new RendererManager();
    }
    return *s_instance;
}

void RendererManager::Destroy()
{
    if (s_instance != nullptr)
    {
        delete s_instance;
        s_instance = nullptr;
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
        std::cerr << "SDL_CreateRenderer failed! Error: " << SDL_GetError() << '\n';
        return;
    }

    SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);
    std::cout << "Renderer succesfully created!" << '\n';
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