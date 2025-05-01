#include "Game.h"

int main(int argc, char* argv[])
{
	Game* game = new Game();

	while (game->Running())
	{
		game->Update();
	}

	delete game;
}
