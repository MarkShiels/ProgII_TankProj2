
#include "TankAi.h"
#include <iostream>

//------------------------------------------------------------

TankAi::TankAi(sf::Texture const & texture, std::vector<sf::Sprite> & wallSprites)
	: m_aiBehaviour(AiBehaviour::SEEK_PLAYER)
	, m_texture(texture)
	, m_wallSprites(wallSprites)
	, m_steering(0, 0)
{
	// Initialises the tank base and turret sprites.
	initSprites();
}

//------------------------------------------------------------

void TankAi::update(Tank & playerTank, float dt)
{
	
	if (collidesWithPlayer(playerTank))
	{
		*m_gameStatePtr = GameState::GAME_LOSE;
	}

	if (*m_gameStatePtr == GameState::GAME_RUNNING)
	{

		if (m_aiState == AiState::PATROL_MAP)
		{
			vectorToPlayer = m_currentPatrolArea - m_tankBase.getPosition();

			switch (m_aiBehaviour)
			{
			case AiBehaviour::SEEK_PLAYER:
				m_steering = sf::Vector2f(0, 0);
				m_steering += thor::unitVector(vectorToPlayer);
				m_steering += collisionAvoidance();
				m_steering = MathUtility::truncate(m_steering, MAX_FORCE);
				m_velocity = MathUtility::truncate(m_velocity + m_steering, MAX_SPEED);  //*7.0f * dt;
				break;

			case AiBehaviour::STOP:
				newPatrol();
				break;

			default:
				break;
			}


			if (thor::length(vectorToPlayer) < MAX_SEE_AHEAD * .25f || stop == true)
			{
				m_aiBehaviour = AiBehaviour::STOP;
			}
			else
			{
				m_aiBehaviour = AiBehaviour::SEEK_PLAYER;
			}

			// Now we need to convert our velocity vector into a rotation angle between 0 and 359 degrees.
			// The m_velocity vector works like this: vector(1,0) is 0 degrees, while vector(0, 1) is 90 degrees.
			// So for example, 223 degrees would be a clockwise offset from 0 degrees (i.e. along x axis).
			// Note: we add 180 degrees below to convert the final angle into a range 0 to 359 instead of -PI to +PI
			auto dest = atan2(-1 * m_velocity.y, -1 * m_velocity.x) / thor::Pi * 180 + 180;

			auto currentRotation = m_rotation;

			// Find the shortest way to rotate towards the player (clockwise or anti-clockwise)
			if (std::round(currentRotation - dest) == 0.0)
			{
				m_steering.x = 0;
				m_steering.y = 0;
			}

			else if ((static_cast<int>(std::round(dest - currentRotation + 360))) % 360 < 180)
			{
				// rotate clockwise
				m_rotation = static_cast<int>((m_rotation)+1) % 360;
			}
			else
			{
				// rotate anti-clockwise
				m_rotation = static_cast<int>((m_rotation)-1) % 360;
			}

		}
		else if (m_aiState == AiState::ATTACK_PLAYER)
		{
			vectorToPlayer = seek(playerTank);

			switch (m_aiBehaviour)
			{
			case AiBehaviour::SEEK_PLAYER:
				m_steering = sf::Vector2f(0, 0);
				m_steering += thor::unitVector(vectorToPlayer);
				m_steering += collisionAvoidance();
				m_steering = MathUtility::truncate(m_steering, MAX_FORCE);
				m_velocity = MathUtility::truncate(m_velocity + m_steering, MAX_SPEED);  //*7.0f * dt;
				break;
			case AiBehaviour::STOP:
				m_velocity = sf::Vector2f(0, 0);
				//motion->m_speed = 0;
			default:
				break;
			}


			if (thor::length(vectorToPlayer) < MAX_SEE_AHEAD * .25f || stop == true)
			{
				m_aiBehaviour = AiBehaviour::STOP;
			}
			else
			{
				m_aiBehaviour = AiBehaviour::SEEK_PLAYER;
			}

			// Now we need to convert our velocity vector into a rotation angle between 0 and 359 degrees.
			// The m_velocity vector works like this: vector(1,0) is 0 degrees, while vector(0, 1) is 90 degrees.
			// So for example, 223 degrees would be a clockwise offset from 0 degrees (i.e. along x axis).
			// Note: we add 180 degrees below to convert the final angle into a range 0 to 359 instead of -PI to +PI
			auto dest = atan2(-1 * m_velocity.y, -1 * m_velocity.x) / thor::Pi * 180 + 180;

			auto currentRotation = m_rotation;

			// Find the shortest way to rotate towards the player (clockwise or anti-clockwise)
			if (std::round(currentRotation - dest) == 0.0)
			{
				m_steering.x = 0;
				m_steering.y = 0;
			}

			else if ((static_cast<int>(std::round(dest - currentRotation + 360))) % 360 < 180)
			{
				// rotate clockwise
				m_rotation = static_cast<int>((m_rotation)+1) % 360;
			}
			else
			{
				// rotate anti-clockwise
				m_rotation = static_cast<int>((m_rotation)-1) % 360;
			}
		}
		
		updateMovement(dt);
	}
}
//------------------------------------------------------------

void TankAi::newPatrol()
{
	int newArea = rand() % 5;
	m_currentPatrolArea = m_patrolAreas[newArea];
	m_aiBehaviour = AiBehaviour::SEEK_PLAYER;
	vectorToPlayer = m_currentPatrolArea - m_tankBase.getPosition();
}

//------------------------------------------------------------

bool TankAi::collidesWithPlayer(Tank const& playerTank) const
{
	// Checks if the AI tank has collided with the player tank.
	if (CollisionDetector::collision(m_turret, playerTank.getTurret()) ||
		CollisionDetector::collision(m_tankBase, playerTank.getTankBase()))
	{
		return true;
	}
	return false;
}

//------------------------------------------------------------

sf::Vector2f TankAi::collisionAvoidance()
{

	//ahead = position + normalize(velocity) * MAX_SEE_AHEAD
		//ahead2 = position + normalize(velocity) * MAX_SEE_AHEAD * 0.5
	// ahead = position + normalize(velocity) * MAX_SEE_AHEAD
	if (m_velocity.x != 0 && m_velocity.y != 0)
	{
		m_ahead = m_tankBase.getPosition() + thor::unitVector(m_velocity) * MAX_SEE_AHEAD;
		float aheadLength = thor::length(m_ahead);
		m_halfAhead = m_tankBase.getPosition() + thor::unitVector(m_velocity) * MAX_SEE_AHEAD;
		m_halfAhead *= 0.5F;
		float halfAheadLength = thor::length(m_halfAhead);
		//auto half = m_ahead * 0.5f;
		//m_halfAhead = m_ahead * 0.5f;
		auto mostThreatening = findMostThreateningObstacle();
		//sf::Vector2f avoidance;
		if (mostThreatening.getRadius() != 0)
		{
			m_avoidance.x = m_ahead.x - mostThreatening.getPosition().x;
			m_avoidance.y = m_ahead.y - mostThreatening.getPosition().y;
			m_avoidance = thor::unitVector(m_avoidance);
			m_avoidance *= MAX_AVOID_FORCE;
			std::cout << "Avoidance: " << m_avoidance.x << "," << m_avoidance.y << std::endl;
		}
		else
		{
			m_avoidance.x = 0;
			m_avoidance.y = 0;
		}
	}

	return m_avoidance;
}

//------------------------------------------------------------

const sf::CircleShape TankAi::findMostThreateningObstacle()
{
	sf::CircleShape mostThreatening;
	mostThreatening.setRadius(0);
	bool collision{ false };

	for (auto obstacle : m_obstacles)
	{
		collision = MathUtility::lineIntersectsCircle(m_ahead, m_halfAhead, obstacle);

		//std::cout << obstacle.getPosition().x << "   " << obstacle.getPosition().y << std::endl;
		//std::cout << m_ahead.x << "," << m_ahead.y << std::endl;
		if (collision /*&& (mostThreatening.getRadius() == 0 ||
			MathUtility::distance(m_tankBase.getPosition(), obstacle.getPosition()) < MathUtility::distance(m_tankBase.getPosition(), mostThreatening.getPosition()))*/)
		{
			mostThreatening = obstacle;
		}
	}

	return mostThreatening;
}

//------------------------------------------------------------

void TankAi::updateMovement(float dt)
{
	double speed = thor::length(m_velocity);
	
	sf::Vector2f newPos(m_tankBase.getPosition().x + std::cos(MathUtility::DEG_TO_RAD * m_rotation) * speed * dt,
	m_tankBase.getPosition().y + std::sin(MathUtility::DEG_TO_RAD * m_rotation) * speed * dt);
	
	m_tankBase.setPosition(newPos.x, newPos.y);
	m_tankBase.setRotation(m_rotation);
	m_turret.setPosition(m_tankBase.getPosition());
	m_turret.setRotation(m_rotation);

}

//------------------------------------------------------------

void TankAi::render(sf::RenderWindow & window)
{
	// TODO: Don't draw if off-screen...

	window.draw(m_tankBase);
	window.draw(m_turret);
}

//------------------------------------------------------------

void TankAi::init(sf::Vector2f position, GameState* t_gameStatePtr, sf::Vector2f t_patrolAreas[5])
{
	for (int i{0}; i < 5; i++)
	{
		m_patrolAreas[i] = t_patrolAreas[i];
		std::cout << m_patrolAreas[i].x << " , "<< m_patrolAreas[i].y << std::endl;
	}

	m_aiState = AiState::PATROL_MAP;

	m_gameStatePtr = t_gameStatePtr;
	m_tankBase.setPosition(position);
	m_turret.setPosition(position);

	m_health = 3;

	newPatrol();

	for (sf::Sprite const wallSprite : m_wallSprites)
	{
		sf::CircleShape circle(wallSprite.getTextureRect().width * 1.5f);
		circle.setOrigin(circle.getRadius(), circle.getRadius());
		circle.setPosition(wallSprite.getPosition());
		m_obstacles.push_back(circle);
	}
}

//------------------------------------------------------------

sf::Vector2f TankAi::seek(Tank&  playerTank)
{
	return playerTank.getPosition() - m_tankBase.getPosition();
}

//------------------------------------------------------------

void TankAi::initSprites()
{
	// Initialise the tank base
	m_tankBase.setTexture(m_texture);
	sf::IntRect baseRect(103, 43, 79, 43);
	m_tankBase.setTextureRect(baseRect);
	m_tankBase.setOrigin(baseRect.width / 2.0, baseRect.height / 2.0);
	m_tankBase.setScale(sf::Vector2f(.8,.8));

	// Initialise the turret
	m_turret.setTexture(m_texture);
	sf::IntRect turretRect(122, 1, 83, 31);
	m_turret.setTextureRect(turretRect);
	m_turret.setOrigin(turretRect.width / 3.0, turretRect.height / 2.0);
	m_turret.setScale(sf::Vector2f(.8, .8));
}

//------------------------------------------------------------

void TankAi::takeDamage()
{
	m_health--;
	damageCounter++;
	std::cout << "AiTank health: " << m_health << std::endl;
	std::cout << "Damage Counter: " << damageCounter << std::endl;
	if(m_health <= 0)
	{
		std::cout << "AiTank health at zero" << std::endl;

		*m_gameStatePtr = GameState::GAME_LOSE;
	}
}

//------------------------------------------------------------

sf::Sprite	TankAi::getTankBase() const
{
	return m_tankBase;
}

//------------------------------------------------------------

sf::Sprite TankAi::getTurret() const
{
	return m_turret;
}

//------------------------------------------------------------

/*void TankAi::updateMovement(float dt)
{
	sf::Vector2f newPos = m_tankBase.getPosition() + m_velocity ;

	auto dest = atan2(-1 * vectorToPlayer.y, -1 * vectorToPlayer.x) / thor::Pi * 180 + 180;

	m_tankBase.setRotation(dest);
	m_turret.setRotation(m_tankBase.getRotation());
	m_tankBase.setPosition(newPos);
	m_turret.setPosition(m_tankBase.getPosition());

}*/

/*
private function findMostThreateningObstacle() :Obstacle{
	var mostThreatening : Obstacle = null;

	for (var i : int = 0; i < Game.instance.obstacles.length; i++) {
		var obstacle : Obstacle = Game.instance.obstacles[i];
		var collision : Boolean = lineIntersecsCircle(ahead, ahead2, obstacle);

		// "position" is the character's current position
		if (collision && (mostThreatening == null || distance(position, obstacle) < distance(position, mostThreatening))) {
			mostThreatening = obstacle;
		}
	}
	return mostThreatening;
}*/

