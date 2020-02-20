#pragma once
#define	_USE_MATH_DEFINES
#include <SFML/Graphics.hpp>
#include "MathUtility.h"
#include <cmath>
#include <iostream>
#include "CollisionDetector.h"
#include "ScreenSize.h"
#include "Target.h"
#include "ProjectilePool.h"
#include "TankAI.h"


/// <summary>
/// @brief A simple tank controller.
/// 
/// This class will manage all tank movement and rotations.
/// </summary>
class Tank
{
public:
	Tank(sf::Texture const& texture,std::vector<sf::Sprite>& wallSprites);;
	void				update(float dt, TankAi & aiTank);
	void				render(sf::RenderWindow& window);
	void				setPosition(sf::Vector2f pos);
	void				increaseSpeed();
	void				decreaseSpeed();
	void				increaseRotation();
	void				decreaseRotation();
	void				increaseTraverse();
	void				decreaseTraverse();
	void				handleKeyIput();
	void				resetTraverse();
	sf::Sprite			getTurret() const;
	sf::Sprite			getTankBase() const;
	static const int	m_numberOfTargetPtrs{ 10 };
	void				passPtrs( Target t_targets[m_numberOfTargetPtrs], int* t_scorePtr, float* t_shotsFired, float* t_hitsMade);
	void				reset();
	sf::Vector2f		getPosition() const;

private:

	// <summary>
/// @brief Checks for collisions between the tank and the walls.
/// 
/// </summary>
/// <returns>True if collision detected between tank and wall.</returns>
	bool checkWallCollision();

	void requestFire();


	void			deflect();
	void			adjustRotation();
	void			resetGun();


	Target*			m_targetPtrs[m_numberOfTargetPtrs];
	int*			m_scorePtr;
	
	bool			m_gunReady{ true };
	int				m_rof{60};


	bool			m_enableRotation{true};
	sf::Vector2f	m_previousPosition;
	double			m_previousSpeed;
	double			m_previousRotation;
	double			m_previousTraverse;


	std::vector<sf::Sprite>&	m_wallSprites;
	bool						mb_resetDirection{ false };
	bool						mb_resetTraverse{false};

	double				m_speed{ 0.0 };
	double				m_rotation{ 0.0 };
	double				m_traverse{ 0.0 };
	void				initSprites();
	
	sf::Sprite			m_tankBase;
	sf::Sprite			m_turret;
	sf::Texture const & m_texture;
	ProjectilePool		m_pool;
	bool				m_fireRequested{false};
	float				m_shootTimer{ 1 };
	static int const	s_TIME_BETWEEN_SHOTS{ 1 };

	int					m_previousHitCount;
	int					m_hitCount;
	std::pair<sf::Sprite, sf::Sprite> aiTankSprites;


};
