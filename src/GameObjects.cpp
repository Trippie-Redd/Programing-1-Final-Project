#include "GameObjects.h"
#include "RendererManager.h"

using namespace Primitives2D;

namespace GameObjects
{
    CompleteTexture s_textures[static_cast<int>(GameObjectsEnum::GAME_OBJECTS_COUNT)];

    //-----------------------------------------------------------------------------
    // Loads all textures in use
    //-----------------------------------------------------------------------------
    void LoadTextures()
    {
        LoadSingleTexture("./assets/key.bmp", GameObjectsEnum::Keys);
        LoadSingleTexture("./assets/ammoCrate.bmp", GameObjectsEnum::AmmoCrates);
    }


    //-----------------------------------------------------------------------------
    // Loads a single texture from a filepath and places it in an array
    // where every spot corresponds to a GameObject type
    //-----------------------------------------------------------------------------
    void LoadSingleTexture(const char* filepath, GameObjectsEnum type)
    {
        SDL_Renderer* renderer = RendererManager::GetInstance().GetRenderer();

        // Attempts to create surface from filepath
        SDL_Surface* surface = SDL_LoadBMP(filepath);
        if (!surface)
        {
            std::cerr << "Unable to load " << filepath << " image! Error: " << SDL_GetError() << '\n';
            return;
        }

        // Attempts to create texture from surface
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (!texture)
        {
            std::cerr << "Unable to create " << filepath << " texture! Error: " << SDL_GetError() << '\n';
            SDL_DestroySurface(surface);
            return;
        }

        // Adds texture to a static array that hold GameObject textures
        s_textures[static_cast<int>(type)] = { texture, Vec2(surface->w, surface->h) };

        SDL_DestroySurface(surface);
    }


    //-----------------------------------------------------------------------------
    // Destroys all loaded SDL textures
    //-----------------------------------------------------------------------------
    void DestroyTextures()
    {
        for (CompleteTexture& texture : s_textures)
        {
            // Only destroy if actually loaded, same as: if (texture.texture != nullptr)
            if (texture.texture)
            {
                SDL_DestroyTexture(texture.texture);
                texture.texture = nullptr;
            }
        }
    }


    //-----------------------------------------------------------------------------
    // Renders a GameObjects texture, takes in ref to Rect but only ever uses
    // position vec2 of Rect, might change
    //-----------------------------------------------------------------------------
    void RenderTexture(const Primitives2D::Rect& object, GameObjectsEnum type)
    {
        SDL_Renderer* renderer = RendererManager::GetInstance().GetRenderer();

        // Creates a rectangle to render to
        SDL_FRect renderQuad = {
            object.min.x,
            object.min.y,
            s_textures[static_cast<int>(type)].dimensions.x,
            s_textures[static_cast<int>(type)].dimensions.y
        };

        // Attempts to render texture
        if (!SDL_RenderTexture(renderer, s_textures[static_cast<int>(type)].texture, NULL, &renderQuad))
        {
            std::cerr << "Unable to render key texture! Error: " << SDL_GetError() << '\n';
        }
    }


    //-----------------------------------------------------------------------------
    // Ammo crate constructor, ammoCount is amount of ammo player will recive
    // upon pickup
    //-----------------------------------------------------------------------------
    AmmoCrate::AmmoCrate(const Vec2& position, uint8_t ammoCount, uint16_t ID)
        : Rect(position, 42, 32)
        , ammoCount(ammoCount)
        , ID(ID)
    {}


    //-----------------------------------------------------------------------------
    // Key constructor, ID used both for checking when loading level as well as 
    // for unlocking locked transition boxes
    //-----------------------------------------------------------------------------
    Key::Key(const Vec2& position, uint16_t ID)
        : Rect(position, 32, 32) // Texture is 32x32 px
        , ID(ID)
    {}


    //-----------------------------------------------------------------------------
    // Transition box constructor, nextLevelID is level that will be loaded when 
    // player collides with box, keyID is which key unlock transition box
    //-----------------------------------------------------------------------------
    TransitionBox::TransitionBox(const Vec2& position, float width, float height, uint16_t nextLevelID, uint16_t keyID)
        : Rect(position, width, height)
        , nextLevelID(nextLevelID)
        , keyID(keyID)
    {}
}