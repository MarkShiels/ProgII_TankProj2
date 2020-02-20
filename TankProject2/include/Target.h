#pragma once
#include <SFML/Graphics.hpp>

class Target
{
public:

	sf::Sprite	targetSprite;
	sf::Texture	targetTexture;


	sf::Vector2f	position;
	bool			inPlay{false};
	bool			destroyed{ false };
	float			lifeSpan{15};
	float			extraLife{0};
	float			spawnTime;

	Target();

	sf::Vector2f	getPosition();
	void			init(sf::Texture t_spriteTexture);
	void			setPosition(sf::Vector2f t_pos);
	
};