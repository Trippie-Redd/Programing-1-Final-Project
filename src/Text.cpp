#include "Text.h"
#include "RendererManager.h"
#include <iostream>

// Initialize static member
TTF_Font* Text::s_font = nullptr;

//-----------------------------------------------------------------------------
// Constructor, creates a new text texture with SDL_Texture, 
// position and dimensions
//-----------------------------------------------------------------------------
Text::Text(const char* text, size_t length, float ptsize, const SDL_Color& color, const Vec2& position)
{
    CreateTextTexture(text, length, ptsize, color, position);
}


//-----------------------------------------------------------------------------
// Destructor, destroys SDL_Texture
//-----------------------------------------------------------------------------
Text::~Text()
{
    ClearTexture();
}


//-----------------------------------------------------------------------------
// Resets texture, position and dimensions
//-----------------------------------------------------------------------------
void Text::ClearTexture()
{
    // If m_texture != nullptr destroy the texture
    if (m_texture)
    {
        SDL_DestroyTexture(m_texture);
        m_texture = nullptr;
    }
    
    // Resets position and dimensions
    m_dimensions = Vec2::Zero();
    m_position   = Vec2::Zero();
}


//-----------------------------------------------------------------------------
// Renders m_texture
//-----------------------------------------------------------------------------
void Text::RenderTexture() const
{
    // Return if m_texture is a nullptr
    if (!m_texture)
    {
        std::cerr << "Cannot render text texture because m_texture is a null pointer!" << '\n';
        return;
    }

    // Create rect to render texture to
    SDL_FRect rect = { m_position.x, m_position.y, m_dimensions.x, m_dimensions.y };

    // Attempts to render the text texture
    if (!SDL_RenderTexture(RendererManager::GetInstance().GetRenderer(), m_texture, nullptr, &rect))
    {
        std::cerr << "SDL_RenderTexture for Text failed! Error: " << SDL_GetError() << '\n';
    }
}


//-----------------------------------------------------------------------------
// Initalizes SDL_TTF and opens a font
// Needa to be called before CreateTextTexture() and RenderTexture()
//-----------------------------------------------------------------------------
bool Text::InitTextEngine()
{
    if (!TTF_Init())
    {
        std::cerr << "TTF_Init failed! Error: " << SDL_GetError() << '\n';
        return false;
    }
    
    // Attempts to open a font to use for the game
    s_font = TTF_OpenFont("./assets/VCR_Font.ttf", 10.0f);
    if (!s_font)
    {
        std::cerr << "Failed to open './assets/VCR_Font.ttf'! Error: " << SDL_GetError() << '\n';
        return false; 
    }

    std::cout << "Text engine initialized!" << '\n';
    return true;
}


//-----------------------------------------------------------------------------
// Closes font and quits SDL_TTF
// Call before exiting program
//-----------------------------------------------------------------------------
void Text::DestroyTextEngine()
{
    if (s_font)
    {
        TTF_CloseFont(s_font);
        s_font = nullptr;
    }

    TTF_Quit();
}


//-----------------------------------------------------------------------------
// Creates a new text texture that can be rendered
//-----------------------------------------------------------------------------
bool Text::CreateTextTexture(const char* text, size_t length, float ptsize, const SDL_Color& color, const Vec2& position)
{
    // Destroys current texture if one has already been created
    if (m_texture)
    {
        SDL_DestroyTexture(m_texture);
        m_texture = nullptr;
    }

    TTF_SetFontSize(s_font, static_cast<int>(ptsize));

    // Attempts to create a surface for texture
    SDL_Surface* textSurface = TTF_RenderText_Solid(s_font, text, length, color);
    if (!textSurface)
    {
        std::cerr << "TTF_RenderText_Blended failed! Error: " << SDL_GetError() << '\n';
        return false;
    }

    // Attempts to create a texture from surface
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(RendererManager::GetInstance().GetRenderer(), textSurface);
    if (!textTexture)
    {
        std::cerr << "SDL_CreateTextureFromSurface failed! Error: " << SDL_GetError() << '\n';
        SDL_DestroySurface(textSurface);
        return false;
    }

    // Gets dimensions from surface for later use then destroys surface
    float width  = static_cast<float>(textSurface->w);
    float height = static_cast<float>(textSurface->h);
    SDL_DestroySurface(textSurface);

    // Assigns correct values to class members
    m_dimensions = Vec2(width, height);
    m_texture = textTexture;
    m_position = position;
    return true;
}