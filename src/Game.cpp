#include "Game.h"

#include "Settings.h"
#include "Text.h"
#include "RendererManager.h"
#include <string>

#include "rapidjson/document.h" 
#include "rapidjson/filereadstream.h" 
#include "rapidjson/filewritestream.h" 
#include "rapidjson/writer.h" 

using namespace Primitives2D;

// ------------- Constructors/Destructor ------------- //

Game::Game()
{
	int flags = 0;
	if (Settings::FULLSCREEN)
	{
		flags = SDL_WINDOW_FULLSCREEN;
	}

	if (!SDL_Init(SDL_INIT_VIDEO))
	{
		std::cerr << "SDL initialization failed! Error: " << SDL_GetError() << '\n';
		return;
	}
	std::cout << "SDL succesfully initialized!" << '\n';

	m_window = SDL_CreateWindow(Settings::TITLE, Settings::WINDOW_WIDTH, Settings::WINDOW_HEIGHT, flags);
	if (!m_window)
	{
		std::cerr << "SDL_CreateWindow failed! Error: " << SDL_GetError() << '\n';
		return;
	}
	std::cout << "Window succesfully created!" << '\n';

	RendererManager::GetInstance().Init(m_window);
	std::cout << "Renderer succesfully created!" << '\n';

	if (!Text::InitTextEngine())
	{
		std::cerr << "TTF_Init failed! Error: " << SDL_GetError() << '\n';
		return;
	}
	std::cout << "TTF text engine succesfully initalized!" << '\n';

	SDL_HideCursor();

	std::fill(&m_unlockedGameObjects[0][0],
		&m_unlockedGameObjects[0][0] + 4 * (UINT16_MAX + 1),
		false);

	LoadLevel(1);

	// For initalizing delta time calculations
	m_lastTime = SDL_GetTicks();

	m_player.SetUnlockedObjects(&m_unlockedGameObjects[0][0]);

	m_isRunning = true;
}

Game::~Game()
{
	RendererManager::Destroy();
	SDL_DestroyWindow(m_window);
	SDL_Quit();

	std::cout << "Game cleaned up!" << '\n';
}

// ----------------- Update/Render ------------------- //

void Game::Update()
{
	// Calculate delta time
	m_currentTime = SDL_GetTicks();
	m_deltaTime = (m_currentTime - m_lastTime) / 1000.0f;
	m_lastTime = m_currentTime;

	HandleEvents();

	m_player.Update(m_environment, m_ammoCrates, m_keys, m_mousePos, m_deltaTime);

	for (Enemy& enemy : m_enemies)
	{
		enemy.Update(m_deltaTime, m_player, m_environment, m_doors);
	}

	Render();
}

void Game::Render()
{
	SDL_Renderer* renderer = RendererManager::GetInstance().GetRenderer();

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Paints canvas white

	SDL_RenderClear(renderer);

	for (const Primitives2D::Rect& wall : m_environment)
	{
		wall.Render(255, 255, 255, 255);
	}

	for (const GameObjects::AmmoCrate ammoCrate : m_ammoCrates)
	{
		ammoCrate.Render(2, 171, 165, 255);
	}

	for (const GameObjects::TransitionBox& transitionBox : m_transitions)
	{
		if (!m_unlockedGameObjects[static_cast<int>(GameObjectsEnum::Keys)][transitionBox.keyID])
			transitionBox.Render(199, 8, 27, 255);
	}

	for (const GameObjects::Key& key : m_keys)
	{
		key.Render(214, 189, 0, 255);
	}

	for (const GameObjects::Door& door : m_doors)
	{
		door.Render(181, 98, 20, 255);
	}

	for (Enemy& enemy : m_enemies)
	{
		enemy.Render();
	}

	m_player.Render();

	SDL_RenderPresent(renderer);
}

// ---------------- Class functions ------------------ //

void Game::HandleEvents()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_EVENT_QUIT:
			m_isRunning = false;
			break;

		case SDL_EVENT_MOUSE_MOTION:
		{
			float x, y;
			SDL_GetMouseState(&x, &y);
			m_mousePos = Vec2(x, y);
			break;
		}

		case SDL_EVENT_MOUSE_BUTTON_DOWN:
			if (!m_mouseButtonPressed)
			{
				m_mouseButtonPressed = true;
				m_player.Shoot(m_environment, m_mousePos);
			}
			break;

		case SDL_EVENT_MOUSE_BUTTON_UP:
			m_mouseButtonPressed = false;
			break;
		}
	}

	// Gets keystate
	const bool* keystate = SDL_GetKeyboardState(NULL);

	// For sprinting input
	if (keystate[SDL_SCANCODE_LSHIFT])
	{
		m_player.setCurrentSpeed(m_player.SPRINTING_SPEED);
	}
	else
	{
		m_player.setCurrentSpeed(m_player.WALKING_SPEED);
	}

	// For reloading
	if (!m_reloadPressed && keystate[SDL_SCANCODE_R])
	{
		m_player.Reload();
		m_reloadPressed = true;
	}
	else if (!keystate[SDL_SCANCODE_R])
	{
		m_reloadPressed = false;
	}

	// Checks for movement input
	if (keystate[SDL_SCANCODE_UP] || keystate[SDL_SCANCODE_W])
	{
		m_player.Move(UP, m_deltaTime);
	}
	if (keystate[SDL_SCANCODE_DOWN] || keystate[SDL_SCANCODE_S])
	{
		m_player.Move(DOWN, m_deltaTime);
	}
	if (keystate[SDL_SCANCODE_LEFT] || keystate[SDL_SCANCODE_A])
	{
		m_player.Move(LEFT, m_deltaTime);
	}
	if (keystate[SDL_SCANCODE_RIGHT] || keystate[SDL_SCANCODE_D])
	{
		m_player.Move(RIGHT, m_deltaTime);
	}
}

void Game::UnloadLevel()
{
	m_environment.clear();
	m_ammoCrates.clear();
	m_transitions.clear();
	m_keys.clear();
	m_doors.clear();
	m_enemies.clear();

	std::cout << "Level unloaded" << '\n';
}

void Game::LoadLevel(uint16_t nexLevelID)
{
	using namespace rapidjson;

	// Try multiple possible paths
	std::string filename = "./levels/level_" + std::to_string(nexLevelID) + ".json";
	FILE* levelJSON = fopen(filename.c_str(), "rb");
	if (!levelJSON)
	{
		std::cerr << "Could not open level file: " << filename << '\n';
		return;
	}

	char* readBuffer = new char[65536];
	FileReadStream is(levelJSON, readBuffer, 65536);

	Document document;
	document.ParseStream(is);
	fclose(levelJSON);

	delete[] readBuffer;

	if (!document.IsObject())
	{
		std::cout << "Invalid JSON format in " << filename << '\n';
		return;
	}

	// Access walls
	const Value& walls = document["walls"];
	for (SizeType i = 0; i < walls.Size(); i++)
	{
		const Value& wall = walls[i];
		int x = wall["x"].GetInt();
		int y = wall["y"].GetInt();
		int width = wall["width"].GetInt();
		int height = wall["height"].GetInt();

		m_environment.push_back(Primitives2D::Rect(Vec2(x, y), width, height));
	}

	// Create wall boundaries
	// dumb
	m_environment.push_back(Rect(Vec2(0, 0), Settings::WINDOW_WIDTH, Settings::BORDER_WALL_THICKNESS));                                                                                                                             // Top wall
	m_environment.push_back(Rect(Vec2(0, Settings::WINDOW_HEIGHT - Settings::BORDER_WALL_THICKNESS), Settings::WINDOW_WIDTH, Settings::BORDER_WALL_THICKNESS));                                                                     // Bottom wall
	m_environment.push_back(Rect(Vec2(0, Settings::BORDER_WALL_THICKNESS), Settings::BORDER_WALL_THICKNESS, Settings::WINDOW_HEIGHT - (Settings::BORDER_WALL_THICKNESS * 2)));                                                      // Left wall
	m_environment.push_back(Rect(Vec2(Settings::WINDOW_WIDTH - Settings::BORDER_WALL_THICKNESS, Settings::BORDER_WALL_THICKNESS), Settings::BORDER_WALL_THICKNESS, Settings::WINDOW_HEIGHT - (Settings::BORDER_WALL_THICKNESS * 2))); // Right wall

	// Access enemies
	const Value& enemies = document["enemies"];
	for (SizeType i = 0; i < enemies.Size(); i++)
	{
		const Value& enemy = enemies[i];
		uint16_t ID = enemy["ID"].GetUint();
		if (m_unlockedGameObjects[static_cast<int>(GameObjectsEnum::Enemies)][ID]) continue;

		std::string type = enemy["type"].GetString();
		int locationX = enemy["locationX"].GetInt();
		int locationY = enemy["locationY"].GetInt();
		int pathStartX = enemy["pathStartX"].GetInt();
		int pathStartY = enemy["pathStartY"].GetInt();
		int pathEndX = enemy["pathEndX"].GetInt();
		int pathEndY = enemy["pathEndY"].GetInt();

		const Vec2 position(locationX, locationY);
		const LineSegment path(Vec2(pathStartX, pathStartY), Vec2(pathEndX, pathEndY));

		m_enemies.push_back(Enemy(Enemy::enemyMap.at(type), position, path, ID));
	}

	// Access ammoCrates
	const Value& ammoCrates = document["ammoCrates"];
	for (SizeType i = 0; i < ammoCrates.Size(); i++)
	{
		const Value& ammoCrate = ammoCrates[i];
		uint16_t ID = ammoCrate["ID"].GetUint();
		if (m_unlockedGameObjects[static_cast<int>(GameObjectsEnum::AmmoCrates)][ID]) continue;

		int x = ammoCrate["x"].GetInt();
		int y = ammoCrate["y"].GetInt();
		uint8_t ammoCount = ammoCrate["ammoCount"].GetUint();

		m_ammoCrates.push_back(GameObjects::AmmoCrate(Vec2(x, y), ammoCount, ID));
	}

	// Access keys
	const Value& keys = document["keys"];
	for (SizeType i = 0; i < keys.Size(); i++)
	{
		const Value& key = keys[i];
		uint16_t ID = key["ID"].GetUint();
		if (m_unlockedGameObjects[static_cast<int>(GameObjectsEnum::Keys)][ID]) continue;

		int x = key["x"].GetInt();
		int y = key["y"].GetInt();

		m_keys.push_back(GameObjects::Key(Vec2(x, y), ID));
	}

	// Access doors
	const Value& doors = document["doors"];
	for (SizeType i = 0; i < doors.Size(); i++)
	{
		const Value& door = doors[i];
		uint16_t ID = door["ID"].GetUint();
		if (m_unlockedGameObjects[static_cast<int>(GameObjectsEnum::Doors)][ID]) continue;

		int x = door["x"].GetInt();
		int y = door["y"].GetInt();
		int width = door["width"].GetInt();
		int height = door["height"].GetInt();

		int hingePosX = door["hingePosX"].GetInt();
		int hingePosY = door["hingePosY"].GetInt();

		m_doors.push_back(GameObjects::Door(Vec2(x, y), width, height, Vec2(hingePosX, hingePosY), ID));
	}

	// Access transition boxes
	const Value& transitionBoxes = document["transitionBoxes"];
	for (SizeType i = 0; i < transitionBoxes.Size(); i++)
	{
		const Value& transitionBox = transitionBoxes[i];

		int x = transitionBox["x"].GetInt();
		int y = transitionBox["y"].GetInt();
		int width = transitionBox["width"].GetInt();
		int height = transitionBox["height"].GetInt();

		uint16_t nextLevelID = transitionBox["nextLevelID"].GetUint();
		uint16_t keyID = transitionBox["keyID"].GetUint();

		m_transitions.push_back(GameObjects::TransitionBox(Vec2(x, y), width, height, nextLevelID, keyID));
	}

	std::cout << filename << " loaded" << '\n';
}