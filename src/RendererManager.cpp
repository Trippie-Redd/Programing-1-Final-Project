#include "RendererManager.h"
#include <iostream>

//-----------------------------------------------------------------------------
// Returns reference to singelton instance
//-----------------------------------------------------------------------------
RendererManager& RendererManager::GetInstance()
{
    static RendererManager instance;
    return instance;
}


//-----------------------------------------------------------------------------
// Initalizes the SDL Renderer ands set draw mode to blend
//-----------------------------------------------------------------------------
bool RendererManager::Init(SDL_Window* window)
{
    // Checks if renderer has already been created
    if (m_renderer != nullptr)
    {
        SDL_DestroyRenderer(m_renderer);
        std::cout << "RendererManager Init has previously been called! Destroying SDL_Renderer and creating new." << '\n';
    }

    m_renderer = SDL_CreateRenderer(window, nullptr);

    // Checks if SDL_CreateRenderer failed
    if (m_renderer == nullptr)
    {
        std::cerr << "SDL_CreateRenderer failed! Error: " << SDL_GetError() << '\n';
        return false;
    }

    SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);
    return true;
}


//-----------------------------------------------------------------------------
// Destructor, destroys SDL_Renderer
//-----------------------------------------------------------------------------
void RendererManager::Destroy()
{
    // Destroys SDL_Renderer
    if (m_renderer != nullptr)
    {
        SDL_DestroyRenderer(m_renderer);
        m_renderer = nullptr;
    }
}


//-----------------------------------------------------------------------------
// Returns pointer to SDL_Renderer in use
//-----------------------------------------------------------------------------
SDL_Renderer* RendererManager::GetRenderer()
{
    // Checks if renderer is initialized
    if (m_renderer == nullptr)
    {
        std::cerr << "Renderer not initialized" << '\n';
    }

    return m_renderer;
}