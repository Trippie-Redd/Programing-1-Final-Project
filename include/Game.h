#pragma once

#include "Player.h"
#include "Enemy.h"
#include "SDL3/SDL.h"
#include "Text.h"

class Game
{
public:
	Game();
	~Game();

	void HandleEvents();
	void Update();
	void Render();

	void LoadLevel(uint16_t nexLevelID);

	bool Running() const;
	std::bitset<65536 * GameObjects::GAME_OBJECTS_ENUM_LENGTH>& GetUnlockedObjects() { return m_unlockedGameObjects;  }

private:
	bool m_isRunning = false;
	bool m_mouseButtonPressed = false;
	bool m_reloadPressed = false;
	SDL_Window* m_window = nullptr;
	Vec2 m_mousePos;
	Player m_player;

	// Lists of all objects in game
	std::vector<Primitives2D::Rect>          m_environment;
	std::vector<GameObjects::AmmoCrate>      m_ammoCrates;
	std::vector<GameObjects::TransitionBox>  m_transitions;
	std::vector<GameObjects::Key>            m_keys;
	std::vector<GameObjects::Door>           m_doors;
	std::vector<Enemy>                       m_enemies;
	std::vector<Text>                        m_text;

	std::bitset<65536*GameObjects::GAME_OBJECTS_ENUM_LENGTH> m_unlockedGameObjects; // uint16_t max value is 65535

	// Delta time vars
	uint32_t m_currentTime = 0;
	uint32_t m_lastTime = 0;
	float m_deltaTime = 0.0f;

};
