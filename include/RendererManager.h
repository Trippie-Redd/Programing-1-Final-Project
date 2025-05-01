#pragma once

#include <SDL3/SDL.h>

class RendererManager
{
public:
    // Get singleton instance
    static RendererManager& GetInstance();

    // Cleanup resources
    static void Destroy();

    // Initialize the renderer
    void Init(SDL_Window* window);

    // Get the renderer
    SDL_Renderer* GetRenderer();

    // Clean up
    ~RendererManager();

private:
    // Private static instance pointer
    static RendererManager* instance;

    // Private renderer pointer
    SDL_Renderer* m_renderer;

    // Private constructor to prevent direct instantiation
    RendererManager() : m_renderer(nullptr) {}

    // Prevent copy and assignment
    RendererManager(const RendererManager&) = delete;
    RendererManager& operator=(const RendererManager&) = delete;
};