#include "Tank.h"

//--------------------------------------------------------------------------------------------------------------------------------------------

Tank::Tank(sf::Texture const& texture, std::vector<sf::Sprite>& wallSprites)
: m_texture(texture), m_wallSprites(wallSprites)
{
	m_previousHitCount = 0;
	m_hitCount = 0;
	initSprites();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Tank::setPosition(sf::Vector2f pos)
{
	m_tankBase.setPosition(pos);
	m_turret.setPosition(pos);
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Tank::update(float dt, TankAi & aiTank)
{	
	aiTankSprites.first = aiTank.getTankBase();
	aiTankSprites.second = aiTank.getTurret();

	if (m_fireRequested)
	{
		m_shootTimer -= dt;
		if (m_shootTimer <= 0)
		{
			m_fireRequested = false;
			m_shootTimer = s_TIME_BETWEEN_SHOTS;
		}
	}

	if (mb_resetTraverse)
	{
		if (mb_resetDirection)
		{
			increaseTraverse();
		}
		else
		{
			decreaseTraverse();
			
		}
	}
	m_tankBase.setRotation(m_rotation);
	
	m_previousPosition = m_tankBase.getPosition();

	float newX = m_tankBase.getPosition().x + std::cos(MathUtility::DEG_TO_RAD * m_rotation) * m_speed * (dt );
	float newY = m_tankBase.getPosition().y + std::sin(MathUtility::DEG_TO_RAD * m_rotation) * m_speed * (dt );

	m_tankBase.setPosition(sf::Vector2f(newX, newY));
	m_turret.setPosition(sf::Vector2f(newX, newY));

	m_turret.setRotation(m_traverse);

	if (m_pool.update(dt, m_wallSprites, aiTankSprites) > 0)
	{
		aiTank.takeDamage();
	}

	/*m_hitCount = m_pool.update(dt, m_wallSprites, aiTankSprites);

	if (m_hitCount > 0)
	{
		aiTank.takeDamage();
	}*/

	if (checkWallCollision())
	{
		deflect();
		m_enableRotation = true;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Tank::render(sf::RenderWindow & window) 
{
	window.draw(m_tankBase);
	window.draw(m_turret);
	m_pool.render(window);
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Tank::requestFire()
{
	m_fireRequested = true;
	if (m_shootTimer == s_TIME_BETWEEN_SHOTS)
	{
		sf::Vector2f tipOfTurret(m_turret.getPosition().x + 2.0f, m_turret.getPosition().y);
		tipOfTurret.x += std::cos(MathUtility::DEG_TO_RAD * m_turret.getRotation()) * ((m_turret.getLocalBounds().top + m_turret.getLocalBounds().height) * 1.7f);
		tipOfTurret.y += std::sin(MathUtility::DEG_TO_RAD * m_turret.getRotation()) * ((m_turret.getLocalBounds().top + m_turret.getLocalBounds().height) * 1.7f);
		m_pool.create(m_texture, tipOfTurret.x, tipOfTurret.y, m_turret.getRotation());
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Tank::deflect()
{
	// In case tank was rotating.
		adjustRotation();

	// If tank was moving.
	if (m_speed != 0)
	{
		// Temporarily disable turret rotations on collision.
		m_enableRotation = false;
		// Back up to position in previous frame.
		m_tankBase.setPosition(m_previousPosition);
		// Apply small force in opposite direction of travel.
		if (m_previousSpeed < 0)
		{
			m_speed = 8;
		}
		else
		{
			m_speed = -8;
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Tank::adjustRotation()
{
	// If tank was rotating...
	if (m_rotation != m_previousRotation)
	{
		// Work out which direction to rotate the tank base post-collision.
		if (m_rotation > m_previousRotation)
		{
			m_rotation = m_previousRotation - 1;
		}
		else
		{
			m_rotation = m_previousRotation + 1;
		}
	}
	// If turret was rotating while tank was moving
	if (m_traverse != m_previousTraverse)
	{
		// Set the turret rotation back to it's pre-collision value.
		m_traverse = m_previousTraverse;
	}

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Tank::initSprites()
{
	// Initialise the tank base
	m_tankBase.setTexture(m_texture);
	sf::IntRect baseRect(2, 43, 79, 43);
	m_tankBase.setTextureRect(baseRect);
	m_tankBase.setOrigin(baseRect.width / 2.0, baseRect.height / 2.0);
	//m_tankBase.setPosition(m_initPos);

	int pos = rand() % 4;

	switch (pos)
	{
	case 0:
		m_tankBase.setPosition(sf::Vector2f(200, 100));
		break;
	case 1:
		m_tankBase.setPosition(sf::Vector2f(800, 100));
		break;
	case 2:
		m_tankBase.setPosition(sf::Vector2f(200, 800));
		break;
	case 3:
		m_tankBase.setPosition(sf::Vector2f(800, 800));
		break;
	}

	// Initialise the turret
	m_turret.setTexture(m_texture);
	sf::IntRect turretRect(19, 1, 83, 31);
	m_turret.setTextureRect(turretRect);
	m_turret.setOrigin(turretRect.width / 3.0, turretRect.height / 2.0);
	m_turret.setPosition(m_tankBase.getPosition());

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Tank::reset()
{

	int pos = rand() % 4;

	switch (pos)
	{
	case 0:
		m_tankBase.setPosition(sf::Vector2f(200, 100));
		break;
	case 1:
		m_tankBase.setPosition(sf::Vector2f(800, 100));
		break;
	case 2:
		m_tankBase.setPosition(sf::Vector2f(200, 800));
		break;
	case 3:
		m_tankBase.setPosition(sf::Vector2f(800, 800));
		break;
	}

	m_previousHitCount = 0;
	m_hitCount = 0;
	m_speed = 0;
	m_rotation = 0;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Tank::passPtrs(Target t_targets[m_numberOfTargetPtrs], int* t_scorePtr, float* t_shotsFired, float* t_hitsMade)
{
	for (int i{ 0 } ; i < m_numberOfTargetPtrs; i++)
	{
		m_targetPtrs[i] = &t_targets[i];
	}

	m_scorePtr = t_scorePtr;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

sf::Vector2f Tank::getPosition() const
{
	return m_tankBase.getPosition();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Tank::increaseSpeed()
{
	m_previousSpeed = m_speed;
	if (m_speed < 100.0)
	{
		m_speed += 1;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Tank::decreaseSpeed()
{
	m_previousSpeed = m_speed;
	if (m_speed > -100.0)
	{
		m_speed -= 1;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Tank::increaseRotation()
{
	//std::cout << "rotation: " << m_rotation << std::endl;
	m_previousRotation = m_rotation;
	m_rotation += 2;
	if (m_rotation > 359.0)
	{
		m_rotation = 0;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Tank::decreaseRotation()
{
	//std::cout << "rotation" << m_rotation << std::endl;
	m_previousRotation = m_rotation;
	m_rotation -= 2;
	if (m_rotation < 0.0)
	{
		m_rotation = 358.0;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Tank::increaseTraverse()
{
	//std::cout << "traverse: " << m_traverse << std::endl;
	m_previousTraverse = m_traverse;
	m_traverse += 2;
	if (m_traverse > 359.0)
	{
		m_traverse = 0;
	}
	if (m_traverse == m_rotation)
	{
		mb_resetTraverse = false;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Tank::decreaseTraverse()
{
	//std::cout << "traverse: " << m_traverse << std::endl;
	m_previousTraverse = m_traverse;
	m_traverse -= 2;
	if (m_traverse < 0.0)
	{
		m_traverse = 358.0;
	}
	if (m_traverse == m_rotation)
	{
		mb_resetTraverse = false;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Tank::handleKeyIput()
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
	{
		increaseSpeed();
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
	{
		decreaseSpeed();
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
	{
		decreaseRotation();
	}	
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
	{
		increaseRotation();
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
	{
		decreaseTraverse();
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
	{
		increaseTraverse();
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::C))
	{
		resetTraverse();
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
	{
		requestFire();
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Tank::resetTraverse()
{

	mb_resetTraverse = true;

	if ((m_traverse - m_rotation) > 180)
	{
		mb_resetDirection = true;
	}
	else if ((m_traverse - m_rotation) < 180)
	{
		mb_resetDirection = false;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

sf::Sprite Tank::getTurret() const
{
	return m_turret;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

sf::Sprite Tank::getTankBase() const
{
	return m_tankBase;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool Tank::checkWallCollision()
{
	for (sf::Sprite const& sprite : m_wallSprites)
	{
		// Checks if either the tank base or turret has collided with the current wall sprite.
		if (CollisionDetector::collision(m_turret, sprite) ||
			CollisionDetector::collision(m_tankBase, sprite))
		{
			return true;
		}
	}
	return false;
}

//--------------------------------------------------------------------------------------------------------------------------------------------
