#include <iostream>

/*
* Things to add: 
* Cursor
* Enemies
* Game
* GameObjects
* Player
* Primitives2D
* Raycast
* Shotgun
* Text
* Settings
*/

#include "SDL3/SDL.h"
#include "SDL3_image/SDL_image.h"
#include "SDL3_ttf/SDL_ttf.h"

int main()
{
	if (SDL_Init(SDL_INIT_VIDEO))
		std::cout << "SDL3 works" << '\n';

	if (TTF_Init())
		std::cout << "TTF works" << '\n';

	std::cout << IMG_Version() << '\n';
	
	return 0;
}