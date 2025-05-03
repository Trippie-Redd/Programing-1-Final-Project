#include "Text.h"
#include "RendererManager.h"

#include <iostream>

TTF_Font* Text::s_font = nullptr;
SDL_Renderer* Text::s_renderer = nullptr;

Text::Text(const char* text, size_t length, float ptsize, const SDL_Color& color, const Vec2& position)
{
    CreateTextTexture(text, length, ptsize, color, position);
}

Text::~Text()
{
    if (m_texture)
    {
        SDL_DestroyTexture(m_texture);
        m_texture = nullptr;
    }
}

void Text::RenderTexture() const
{
    if (!m_texture)
    {
        std::cerr << "Cannot render text texture because m_texture is a null pointer!" << '\n';
        return;
    }

    SDL_Renderer* renderer = s_renderer;
    SDL_FRect rect = { m_position.x, m_position.y, m_dimensions.x, m_dimensions.y };
    
    /*
    * FIXME : m_texture is an invalid argument
    * idk what happened but ts works now for some reason
    */
    if (!SDL_RenderTexture(renderer, m_texture, nullptr, &rect))
    {
        std::cerr << "SDL_RenderTexture for Text failed! Error: " << SDL_GetError() << '\n';
    }
}

bool Text::InitTextEngine()
{
    if (!TTF_Init())
    {
        std::cerr << "TTF_Init failed! Error: " << SDL_GetError() << '\n';
        return false;
    }

    s_font = TTF_OpenFont("./assets/VCR_Font.ttf", 10.0f);
    if (!s_font)
    {
        std::cerr << "TTF_OpenFont failed! Error: " << SDL_GetError() << '\n';
        return false;
    }

    s_renderer = RendererManager::GetInstance().GetRenderer();

    std::cout << "Text engine initialized!" << '\n';
    return true;
}

void Text::DestroyTextEngine()
{
    if (s_font)
    {
        TTF_CloseFont(s_font);
        s_font = nullptr;
    }
}

bool Text::CreateTextTexture(const char* text, size_t length, float ptsize, const SDL_Color& color, const Vec2& position)
{
    if (!s_font || !s_renderer)
    {
        std::cerr << "Font or renderer not initialized!" << '\n';
        return false;
    }

    if (m_texture)
    {
        SDL_DestroyTexture(m_texture);
        m_texture = nullptr;
    }

    if (!TTF_SetFontSize(s_font, ptsize))
    {
        std::cerr << "TTF_SetFontSize failed! Error: " << SDL_GetError() << '\n';
        return false;
    }

    SDL_Surface* textSurface = TTF_RenderText_Blended(s_font, text, length, color);
    if (!textSurface)
    {
        std::cerr << "TTF_RenderText_Blended failed! Error: " << SDL_GetError() << '\n';
        return false;
    }

    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(s_renderer, textSurface);
    if (!textTexture)
    {
        std::cerr << "SDL_CreateTextureFromSurface failed! Error: " << SDL_GetError() << '\n';
        SDL_DestroySurface(textSurface);
        return false;
    }

    float width =  static_cast<float>(textSurface->w);
    float height = static_cast<float>(textSurface->h);
    SDL_DestroySurface(textSurface);

    m_dimensions = Vec2(width, height);
    m_position = position;
    m_texture = textTexture;

    return true;
}
