#pragma once

#include "Vec2.h"
#include "SDL3_ttf/SDL_ttf.h"

class Text
{
public:
    Text(const char* text, size_t length, float ptsize, const SDL_Color& color, const Vec2& position);
    ~Text();

    bool RenderTexture() const;
    bool CreateTextTexture(const char* text, size_t length, float ptsize, const SDL_Color& color, const Vec2& position);
    bool ChangeColor(SDL_Color color)                { return CreateTextTexture(m_text, m_length, m_ptsize, color, m_position); }
    bool ChangeFontSize(float ptsize)                { return CreateTextTexture(m_text, m_length, ptsize, m_color, m_position); }
    bool ChangeText(const char* text, size_t length) { return CreateTextTexture(text, length, m_ptsize, m_color, m_position);   }
    bool ChangePosition(const Vec2& position)        { return CreateTextTexture(m_text, m_length, m_ptsize, m_color, position); }

    static bool InitTextEngine();
    static void DestroyTextEngine();

private:
    static TTF_Font* s_font;
    static SDL_Renderer* s_renderer;

    const char* m_text;
    size_t m_length;
    float m_ptsize;
    Vec2 m_position;
    Vec2 m_dimensions;
    SDL_Color m_color;
    SDL_Texture* m_texture;

};