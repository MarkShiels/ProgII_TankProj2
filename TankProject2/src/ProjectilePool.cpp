#include "ProjectilePool.h"

//--------------------------------------------------------------------------------------------------------------------------------------------

void ProjectilePool::create(sf::Texture const & texture, double x, double y, double rotation)
{
	// If no projectiles available, simply re-use the next in sequence.
	if (m_poolFull)
	{
		m_nextAvailable = (m_nextAvailable + 1) % s_POOL_SIZE;
	}
	
	m_projectiles.at(m_nextAvailable).init(texture, x, y, rotation);
}

//--------------------------------------------------------------------------------------------------------------------------------------------

int ProjectilePool::update(double dt, std::vector<sf::Sprite> & wallSprites, std::pair<sf::Sprite, sf::Sprite> aiTankSprites)
{	
	// The number of active projectiles.
	m_activeCount = 0;

	m_hitCount = 0;

	// Assume the pool is not full initially.
	m_poolFull = false;
	for (int i = 0; i < s_POOL_SIZE; i++)
	{
		result = m_projectiles.at(i).update(dt, wallSprites, aiTankSprites);
		//std::cout << "Pool loop: " << i << std::endl;
		if (result.first)
		{
			// If this projectile has expired, make it the next available.
			m_nextAvailable = i;
		}
		else
		{
			// So we know how many projectiles are active.
			m_activeCount++;
		}

		if (result.second)
		{
			m_hitCount++;
		}

		//std::cout << "Active Count: " << m_activeCount << std::endl;
	}
	// If no projectiles available, set a flag.
	if (m_activeCount == s_POOL_SIZE )
	{		
		m_poolFull = true;
	}

	return m_hitCount;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void ProjectilePool::render(sf::RenderWindow & window)
{
	for (int i = 0; i < s_POOL_SIZE; i++)
	{
		// If projectile is active...
		if (m_projectiles.at(i).inUse() )
		{
			window.draw(m_projectiles.at(i).m_projectile);
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------