#pragma once

#include "Vec2.h"
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3/SDL.h>

class Text
{
public:
    Text() = default;
    Text(const char* text, size_t length, float ptsize, const SDL_Color& color, const Vec2& position);
    ~Text();

    void RenderTexture() const;
    bool CreateTextTexture(const char* text, size_t length, float ptsize, const SDL_Color& color, const Vec2& position);
    void ClearTexture();
    bool IsNull() const { return m_texture == nullptr;  };

    static bool InitTextEngine();
    static void DestroyTextEngine();

private:
    static TTF_Font* s_font;

    SDL_Texture* m_texture = nullptr;
    Vec2 m_position;
    Vec2 m_dimensions;
};
