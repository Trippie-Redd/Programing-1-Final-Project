#include "Text.h"
#include "RendererManager.h"

#include <iostream>

TTF_Font* Text::s_font = nullptr;

Text::Text(const char* text, size_t length, float ptsize, const SDL_Color& color, const Vec2& position)
{
    CreateTextTexture(text, length, ptsize, color, position);
}

Text::~Text()
{
    SDL_DestroyTexture(m_texture);
}

bool Text::RenderTexture() const
{
    SDL_Renderer* renderer = RendererManager::GetInstance().GetRenderer();
    const SDL_FRect rect = { m_position.x, m_position.x, m_dimensions.x, m_dimensions.y };
    return SDL_RenderTexture(renderer, m_texture, NULL, &rect);
}

bool Text::InitTextEngine()
{
	if (!TTF_Init())
	{
		std::cerr << "TTF_Init for InitTextEngine could not be created! Error: " << SDL_GetError() << '\n';
		return false;
	}

    s_font = TTF_OpenFont("./assets/VCR_Font.ttf", 10.0f);
	if (!s_font)
	{
		std::cerr << "TTF_OpenFont for InitTextEngine failed! Error: " << SDL_GetError() << '\n';
		return false;
	}

	std::cout << "Text engine initialized!" << '\n';
    return true;
}

void Text::DestroyTextEngine()
{
    TTF_CloseFont(s_font);
}

bool Text::CreateTextTexture(const char* text, size_t length, float ptsize, const SDL_Color& color, const Vec2& position)
{
    if (!TTF_SetFontSize(s_font, ptsize))
    {
        std::cerr << "TTF_SetFontSize for GetTextureFromText could not be created! Error: " << SDL_GetError() << '\n';
        return false;
    }

    SDL_Surface* textSurface = TTF_RenderText_Blended(s_font, text, length, color);
    if (!textSurface)
    {
        std::cerr << "SDL_Surface for GetTextureFromText could not be created! Error: " << SDL_GetError() << '\n';
        return false;
    }

    SDL_Renderer* renderer = RendererManager::GetInstance().GetRenderer();
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    if (!textTexture)
    {
        std::cerr << "SDL_Texture for GetTextureFromText could not be created! Error: " << SDL_GetError() << '\n';
        SDL_DestroySurface(textSurface);
        return false;
    }

    float textWidth = static_cast<float>(textSurface->w);
    float textHeight = static_cast<float>(textSurface->h);

    SDL_DestroySurface(textSurface);

    m_texture = textTexture;
    m_dimensions = { textWidth, textHeight };
    m_color = color;
    m_position = position;
    m_text = text;
    m_length = length;
    m_ptsize = ptsize;

    return true;
}
