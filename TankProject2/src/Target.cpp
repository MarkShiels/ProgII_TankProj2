#include "Target.h"

Target::Target()
{
	
}

void Target::init(sf::Texture t_spriteTexture)
{
	targetTexture = t_spriteTexture;
	targetSprite.setTexture(targetTexture);
}

void Target::setPosition(sf::Vector2f t_pos)
{
	targetSprite.setPosition(t_pos);
}

sf::Vector2f Target::getPosition()
{
	return position;
}
