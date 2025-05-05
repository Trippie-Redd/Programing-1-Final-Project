#include "GameObjects.h"
#include "RendererManager.h"
/*
* 
* Maybe move all texture stuff into separate 
* class/singleton if it grows to expansive,
* probably do this
* 
* texture ideas:
* store them in an unordered_map<std::string/const char* name, SDL_Texture* texture>
*               or unordered_map<Enum name, SDL_Texture* texture>
* 
*/
using namespace Primitives2D;

namespace GameObjects
{
    void LoadTextures()
    {
        LoadSingleTexture("./assets/key.bmp", GameObjectsEnum::Keys);
        LoadSingleTexture("./assets/ammoCrate.bmp", GameObjectsEnum::AmmoCrates);
    }

    void LoadSingleTexture(const char* filepath, GameObjectsEnum type)
    {
        SDL_Renderer* renderer = RendererManager::GetInstance().GetRenderer();

        SDL_Surface* surface = SDL_LoadBMP(filepath);
        if (!surface)
        {
            std::cerr << "Unable to load " << filepath << " image! Error: " << SDL_GetError() << '\n';
            return;
        }

        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (!texture)
        {
            std::cerr << "Unable to create " << filepath << " texture! Error: " << SDL_GetError() << '\n';
            SDL_DestroySurface(surface);
            return;
        }

        s_textures[static_cast<int>(type)] = { texture, Vec2(surface->w, surface->h) };

        SDL_DestroySurface(surface);
    }

    void DestroyTextures()
    {
        for (CompleteTexture texture : s_textures)
        {
            if (texture.texture)
            {
                SDL_DestroyTexture(texture.texture);
            }
        }
    }

    // Not a big fan of this but it's alright
    void RenderTexture(const Primitives2D::Rect& object, GameObjectsEnum type)
    {
        SDL_Renderer* renderer = RendererManager::GetInstance().GetRenderer();

        SDL_FRect renderQuad = {
            object.min.x,
            object.min.y,
            s_textures[static_cast<int>(type)].dimensions.x,
            s_textures[static_cast<int>(type)].dimensions.y
        };

        if (!SDL_RenderTexture(renderer, s_textures[static_cast<int>(type)].texture, NULL, &renderQuad))
        {
            std::cerr << "Unable to render key texture! Error: " << SDL_GetError() << '\n';
        }
    }

    AmmoCrate::AmmoCrate(const Vec2& position, uint8_t ammoCount, uint16_t ID)
        : Rect(position, 42, 32)
        , ammoCount(ammoCount)
        , ID(ID)
    {}

    Key::Key(const Vec2& position, uint16_t ID)
        : Rect(position, 32, 32) // Texture is 32x32 px
        , ID(ID)
    {}
    
    Door::Door(const Vec2& position, float width, float height, const Vec2& hingePos, uint16_t ID)
        : Rect(position, width, height)
        , hingePos(hingePos)
        , ID(ID)
    {}

    TransitionBox::TransitionBox(const Vec2& position, float width, float height, uint16_t nextLevelID, uint16_t keyID)
        : Rect(position, width, height)
        , nextLevelID(nextLevelID)
        , keyID(keyID)
    {}
}