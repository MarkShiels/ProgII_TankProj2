#include "Game.h"
#include <iostream>

// Updates per milliseconds
static double const MS_PER_UPDATE = 10.0;

//--------------------------------------------------------------------

Game::Game()
	: m_window(sf::VideoMode(ScreenSize::s_height, ScreenSize::s_width, 32), "SFML Playground", sf::Style::Default)
	,m_tank(m_texture, m_wallSprites), m_aiTank(m_texture, m_wallSprites)
	, m_hud(m_font)
{
	m_window.setVerticalSyncEnabled(true);

	srand(time(NULL));

	if (!m_backgroundTexture.loadFromFile("./resources/images/Background.jpg"))
	{
	}
	m_backgroundSprite.setTexture(m_backgroundTexture);

	if (!m_font.loadFromFile("./resources/fonts/ariblk.ttf"))
	{
		std::string s("Error loading font");
		throw std::exception(s.c_str());
	}
	int currentLevel = 1;

	try
	{
		LevelLoader::load(currentLevel, m_level);
	}
	catch(std::exception & e)
	{
		std::cout << "Level Loading Failure" << std::endl;
		std::cout << e.what() << std::endl;
		throw e;
	}

	if (!m_texture.loadFromFile("./resources/images/SpriteSheet.png"))
	{
		std::string s("Error loading texture");
		throw std::exception(s.c_str());
	}

	if (!m_bulletTexture.loadFromFile("./resources/images/SpriteSheet.png"))
	{
		std::string s("Error loading texture");
		throw std::exception(s.c_str());
	}

	if (!m_targetTexture.loadFromFile("./resources/images/target.png"))
	{
		std::string s("Error loading texture");
		throw std::exception(s.c_str());
	}

	if (!m_clockFont.loadFromFile("./resources/Fonts/ariblk.ttf"))
	{
	}

	m_clockText.setFont(m_clockFont);
	m_clockText.setPosition(10, 10);
	m_clockText.setOutlineColor(sf::Color::Black);
	m_clockText.setOutlineThickness(2);
	m_clockText.setCharacterSize(24);

	m_gameOverText.setFont(m_clockFont);
	m_gameOverText.setPosition(600, 350);
	m_gameOverText.setCharacterSize(28);

	m_scoreText.setFont(m_clockFont);
	m_scoreText.setOutlineColor(sf::Color::Black);
	m_scoreText.setOutlineThickness(2);
	m_scoreText.setPosition(10, 40);
	m_scoreText.setCharacterSize(24);

	generateWalls();	

	init();
}

//--------------------------------------------------------------------

void Game::init()
{

	m_gameState = GameState::GAME_RUNNING;

	m_shotsFired = 0;
	m_hitsMade = 0;

	m_shotsFiredPtr = &m_shotsFired;
	m_hitsMadePtr = &m_hitsMade;

	for (int i{ 0 }; i < m_numberOfTargets; i++)
	{
		m_targets[i].init(m_targetTexture);
		m_targets[i].spawnTime = m_targetspawnInterval * i;
	}

	m_targets[0].inPlay = true;
	do
	{
		{
			m_targets[0].setPosition(sf::Vector2f(rand() % 1380 + 20, rand() % 860 + 20));
		}
	} while (targetSpawnCollision(m_targets[0].targetSprite));

	m_restartDelay = sf::seconds(3);

	m_playerScore = 0;
	m_playerScorePtr = &m_playerScore;

	m_clockTime = 60;
	m_clockText.setString("Time: " + std::to_string(static_cast<int>(m_clockTime)));
	m_scoreText.setString("Score: " + std::to_string(m_playerScore));

	m_tank.reset();
	m_tank.passPtrs(m_targets, m_playerScorePtr, m_shotsFiredPtr, m_hitsMadePtr);

	m_aiTank.init(m_level.m_aiTank.m_position, &m_gameState);
}

//--------------------------------------------------------------------

void Game::run()
{
	/*sf::Clock clock;
	sf::Int32 lag = 0;

	while (m_window.isOpen())
	{
		sf::Time dt = clock.restart();

		lag += dt.asMilliseconds();

		processEvents();

		while (lag > MS_PER_UPDATE)
		{
			update(MS_PER_UPDATE);
			lag -= MS_PER_UPDATE;
		}
		update(MS_PER_UPDATE);

		render();
	}*/

	sf::Clock clock;
	sf::Time timeSinceLastUpdate = sf::Time::Zero;
	sf::Time timePerFrame = sf::seconds(1.f / 60.f);

	while (m_window.isOpen())
	{
		processEvents();
		timeSinceLastUpdate += clock.restart();
		while (timeSinceLastUpdate > timePerFrame)
		{
			timeSinceLastUpdate -= timePerFrame;
			update(timePerFrame);
		}
		render();
	}
}

//--------------------------------------------------------------------

void Game::processEvents()
{
	sf::Event event;
	while (m_window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
		{
			m_window.close();
		}
		//processGameEvents(event);
	}

	m_tank.handleKeyIput();
}

//--------------------------------------------------------------------

void Game::update(sf::Time dt)
{
	m_clockTime -= 1.0f/150.0f;
	m_clockText.setString("Time: " + std::to_string(static_cast<int>(m_clockTime)));

	if (m_gameState == GameState::GAME_RUNNING)
	{
		if (m_clockTime < 0)
		{
			m_gameState = GameState::GAME_LOSE;
		}

	
		m_tank.update(dt.asSeconds(), m_aiTank);
		m_aiTank.update(m_tank, dt.asSeconds());
		m_hud.update(m_gameState);

		m_scoreText.setString("Score: " + std::to_string(m_playerScore));

		spawnTargets();
	}
	else if (m_gameState == GameState::GAME_LOSE)
	{
		m_restartDelay -= dt;
		if (m_restartDelay <= sf::seconds(0))
		{
			init();
		}
	}
}

//--------------------------------------------------------------------

void	Game::spawnTargets()
{
	for (int i{ 0 }; i < m_numberOfTargets; i++)
	{
		if (m_targets[i].inPlay)
		{
			if (targetSpawnCollision(m_targets[i].targetSprite))
			{
				do
				{
					m_targets[i].setPosition(sf::Vector2f(rand() % 1380 + 20, rand() % 860 + 20));

				} while (targetSpawnCollision(m_targets[i].targetSprite));
			}

			m_targets[i].lifeSpan -= 1.0f / 150.0f;

			if (m_targets[i].lifeSpan <= 0)
			{
				m_targets[i].inPlay = false;
				m_targets[i].destroyed = true;
				m_targets[i].setPosition(sf::Vector2f(-200, -200));
			}
		}
		else if(!m_targets[i].inPlay && !m_targets[i].destroyed)
		{
			m_targets[i].spawnTime -= 1.0f / 150.0f;
			if (m_targets[i].spawnTime <= 0)
			{
				m_targets[i].inPlay = true;
				m_targets[i].setPosition(sf::Vector2f(rand() % 1380 + 20, rand() % 860 + 20));
			}
		}


	}
		
	/*	m_targetOne.lifeSpan -= 1.0f / 150.0f;
	
	if (m_targetOne.lifeSpan <= 0)
	{
		m_targetOne.inPlay = false;
	}

	if (!m_targetOne.inPlay)
	{
		m_targetOne.setPosition(sf::Vector2f(rand() % 1380 + 20, rand() % 850 + 20));
		if (targetSpawnCollision(m_targetOne.targetSprite))
		{
			m_targetOne.setPosition(sf::Vector2f(rand() % 1380 + 20, rand() % 850 + 20));
		}
		else
		{
			m_targetOne.inPlay = true;
			m_targetOne.lifeSpan = m_targetOne.extraLife + 10.0f;
			m_targetOne.extraLife = 0;
		}
	}*/
}

//--------------------------------------------------------------------

bool	Game::targetSpawnCollision(sf::Sprite t_target)
{
	for (sf::Sprite const& sprite : Game::m_wallSprites)
	{
		if (CollisionDetector::collision(sprite, t_target))
		{
			return true;
		}
	
	}

	return false;
}

//--------------------------------------------------------------------

void Game::generateWalls()
{
	sf::Sprite sprite;
	sf::IntRect wallRect(2, 129, 33, 23);
	sprite.setTexture(m_texture);
	sprite.setTextureRect(wallRect);

	// Loop through each Obstacle instance - recall that Obstacles are structs
	for (auto& obstacle : m_level.m_obstacles)
	{
		// Position the wall sprite using the obstacle data
		sprite.setPosition(obstacle.m_position);
		sprite.setRotation(obstacle.m_rotation);
		m_wallSprites.push_back(sprite);
	}
}

//--------------------------------------------------------------------

void Game::setMoveSpeed()
{
	moveSpeed = 1;
}

//--------------------------------------------------------------------

void Game::render()
{
	m_window.clear(sf::Color(0, 0, 0, 0));

	if (m_gameState == GameState::GAME_LOSE)
	{
		if (m_hitsMade == 0 || m_shotsFired == 0)
		{
			m_playerAccuracy = 0;
		}
		else
		{
			m_playerAccuracy = (m_hitsMade / m_shotsFired) * 100;
		}
		m_gameOverText.setString("Game Over\nYour Score: " + std::to_string(m_playerScore) + "\nYour accuracy:  " + std::to_string(m_playerAccuracy) + "%");
		m_window.draw(m_gameOverText);
		m_hud.render(m_window);
	}
	else
	{
		m_window.draw(m_backgroundSprite);
		
		for (int i{ 0 }; i < m_wallSprites.size(); i++)
		{
			m_window.draw(m_wallSprites[i]);
		}

		for (int i{ 0 }; i < m_numberOfTargets; i++)
		{
			if (m_targets[i].inPlay)
			{
				m_window.draw(m_targets[i].targetSprite);
			}
		}

		//if (m_targetOne.getInPlay())
		//{
		//	m_window.draw(m_targetOne.targetSprite);
		//}
		
		m_hud.render(m_window);
		m_tank.render(m_window);
		m_aiTank.render(m_window);
		m_window.draw(m_clockText);
		m_window.draw(m_scoreText);
	}
	
	m_window.display();
}

//--------------------------------------------------------------------



