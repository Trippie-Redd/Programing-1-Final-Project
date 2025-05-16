#pragma once

#include <SDL3/SDL.h>

class RendererManager
{
public:
    static RendererManager& GetInstance();
    bool Init(SDL_Window* window);
    void Destroy();

    SDL_Renderer* GetRenderer();

private:
    RendererManager() : m_renderer(nullptr) {}
    ~RendererManager() = default;

    SDL_Renderer* m_renderer;

private:
    // Prevent copy and assignment
    RendererManager(const RendererManager&)            = delete;
    RendererManager& operator=(const RendererManager&) = delete;
};