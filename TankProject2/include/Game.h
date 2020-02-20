#pragma once

#include "Tank.h"
#include "TankAI.h"
#include "HUD.h"
#include <SFML/Graphics.hpp>
#include "ScreenSize.h"
#include <LevelLoader.h>
#include <time.h>
#include "Target.h"

/// <summary>
/// @brief Main class for the SFML Playground project.
/// 
/// This will be a single class framework for learning about SFML. 
/// Example usage:
///		Game game;
///		game.run();
/// </summary>

class Game
{
public:
	/// <summary>
	/// @brief Default constructor that initialises the SFML window, 
	///   and sets vertical sync enabled. 
	/// </summary>
	Game();


	/// <summary>
	/// @brief the main game loop.
	/// 
	/// A complete loop involves processing SFML events, updating and drawing all game objects.
	/// The actual elapsed time for a single game loop results (lag) is stored. If this value is 
	///  greater than the notional time for one loop (MS_PER_UPDATE), then additional updates will be 
	///  performed until the lag is less than the notional time for one loop.
	/// The target is one update and one render cycle per game loop, but slower PCs may 
	///  perform more update than render operations in one loop.
	/// </summary>
	void run();
	static const int	m_numberOfTargets{ 10 };
	const float	m_targetspawnInterval{ 10 };
	std::vector<sf::Sprite> m_wallSprites;
	
protected:
	/// <summary>
	/// @brief Placeholder to perform updates to all game objects.
	/// </summary>
	/// <param name="time">update delta time</param>
	void update(sf::Time dt);

	/// <summary>
	/// @brief Draws the background and foreground game objects in the SFML window.
	/// The render window is always cleared to black before anything is drawn.
	/// </summary>
	void render();

	/// <summary>
	/// @brief Checks for events.
	/// Allows window to function and exit. 
	/// Events are passed on to the Game::processGameEvents() method.
	/// </summary>	
	void processEvents();

	/// <summary>
	/// @brief Handles all user input.
	/// </summary>
	/// <param name="event">system event</param>
	//void processGameEvents(sf::Event&);

	void init();

	void generateWalls();
	void setMoveSpeed();

	// main window
	sf::RenderWindow m_window;
	int moveSpeed{ 0 };


	bool		targetSpawnCollision(sf::Sprite t_target);

	void		spawnTargets();

	Target		m_targetOne;
	Target*		m_targetOnePtr;

	Target		m_targets[m_numberOfTargets];
	Target*		m_targetPtrs[m_numberOfTargets];

	Tank		m_tank;

	TankAi		m_aiTank;

	int			m_playerAccuracy;
	float		m_shotsFired;
	float		m_hitsMade;
	float*		m_shotsFiredPtr;
	float*		m_hitsMadePtr;
	int			m_playerScore;
	int*		m_playerScorePtr;
	sf::Text	m_scoreText;

	LevelData	m_level;

	TargetData	m_target;

	sf::Texture m_texture;
	sf::Texture m_backgroundTexture;
	sf::Sprite	m_backgroundSprite;
	sf::Texture	m_bulletTexture;
	sf::Texture	m_targetTexture;

	sf::Font	m_clockFont;
	sf::Text	m_clockText;
	float		m_clockTime;

	sf::Text	m_gameOverText;

	GameState m_gameState{ GameState::GAME_RUNNING };

	// The font to be used for the HUD.
	sf::Font m_font;

	// The game HUD instance.
	HUD m_hud;

	sf::Time	m_restartDelay;

};
