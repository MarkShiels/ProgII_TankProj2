#pragma once
#include "MathUtility.h"
#include "GameState.h"
#include "CollisionDetector.h"
#include <SFML/Graphics.hpp>
#include <Thor/Vectors.hpp>
#include <iostream>
#include <queue>

class Tank;

class TankAi
{
public:
	/// <summary>
	/// @brief Constructor that stores a reference to the obstacle container.
	/// Initialises steering behaviour to seek (player) mode, sets the AI tank position and
	///  initialises the steering vector to (0,0) meaning zero force magnitude.
	/// </summary>
	/// <param name="texture">A reference to the sprite sheet texture</param>
	///< param name="wallSprites">A reference to the container of wall sprites</param>
	TankAi(sf::Texture const & texture, std::vector<sf::Sprite> & wallSprites);

	/// <summary>
	/// @brief Steers the AI tank towards the player tank avoiding obstacles along the way.
	/// Gets a vector to the player tank and sets steering and velocity vectors towards
	/// the player if current behaviour is seek. If behaviour is stop, the velocity vector
	/// is set to 0. Then compute the correct rotation angle to point towards the player tank. 
	/// If the distance to the player tank is < MAX_SEE_AHEAD, then the behaviour is changed from seek to stop.
	/// Finally, recalculate the new position of the tank base and turret sprites.
	/// </summary>
	/// <param name="playerTank">A reference to the player tank</param>
	/// <param name="dt">update delta time</param>
	void update(Tank& playerTank, float dt);

	/// <summary>
	/// @brief Draws the tank base and turret.
	///
	/// </summary>
	/// <param name="window">The SFML Render window</param>
	void render(sf::RenderWindow & window);

	/// <summary>
	/// @brief Initialises the obstacle container and sets the tank base/turret sprites to the specified position.
	/// <param name="position">An x,y position</param>
	/// </summary>
	void init(sf::Vector2f position, GameState* t_gameStatePtr);

	enum class AiType
	{
		AI_ID_NONE,
		AI_ID_SEEK_SHOOT_AT_PLAYER
	};

	sf::Sprite	getTankBase() const;
	sf::Sprite  getTurret() const;

	int			m_health;
	void		takeDamage();

private:

	void initSprites();

	void updateMovement(float dt);

	sf::Vector2f seek(Tank& playerTank);

	sf::Vector2f collisionAvoidance();

	const sf::CircleShape findMostThreateningObstacle();


	// A reference to the sprite sheet texture.
	sf::Texture const & m_texture;

	// A sprite for the tank base.
	sf::Sprite m_tankBase;

	// A sprite for the turret
	sf::Sprite m_turret;

	// A reference to the container of wall sprites.
	std::vector<sf::Sprite> & m_wallSprites;

	float m_previousRotation;

	// The current rotation as applied to tank base and turret.
	float m_rotation{ 0.0 };

	// Current velocity.
	sf::Vector2f m_velocity;

	// Steering vector.
	sf::Vector2f m_steering;

	// The ahead vector.
	sf::Vector2f m_ahead;

	// The half-ahead vector.
	sf::Vector2f m_halfAhead;

	//The vector from Aitank to Player Tank
	sf::Vector2f vectorToPlayer;

	bool stop{ false };

	//Avoidance vector
	sf::Vector2f m_avoidance;

	// The maximum see ahead range.
	static float constexpr MAX_SEE_AHEAD{ 100.0f };

	// The maximum avoidance turn rate.
	static float constexpr MAX_AVOID_FORCE{ 100.0f };

	// 
	static float constexpr MAX_FORCE{ 120.0f };

	// The maximum speed for this tank.
	float MAX_SPEED = 100.0f;

	// A container of circles that represent the obstacles to avoid.
	std::vector<sf::CircleShape> m_obstacles;

	enum class AiBehaviour
	{
		SEEK_PLAYER,
		STOP,
		RETREAT
	} m_aiBehaviour;
	
	// <summary>
/// @brief Checks for collision between the AI and player tanks.
///
/// </summary>
/// <param name="player">The player tank instance</param>
/// <returns>True if collision detected between AI and player tanks.</returns>
	bool collidesWithPlayer(Tank const& playerTank) const;

	GameState* m_gameStatePtr;

	int			damageCounter;
};

#include "Tank.h"

