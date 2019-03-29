#include "Explosion.h"
#include "Enemy.h"
#include <boost/algorithm/clamp.hpp>


Explosion::Explosion()
{;
	LoadResources();
	spriteRect = sf::IntRect(0, 0, 128, 128);
	tex2d.sprite.setTextureRect(spriteRect);
}

Explosion::Explosion(sf::Vector2f position, int rowid, int maxShootTime)
{;
	LoadResources();
	spriteRect = sf::IntRect(0, 0, 128, 128);
	tex2d.sprite.setTextureRect(spriteRect);
	pos = position;
	tex2d.sprite.setPosition(sf::Vector2f(position.x, position.y));
}

Explosion& Explosion::operator=(const Explosion& other)
{
	pos = other.pos;
	return *this;
}

Explosion::~Explosion()
{
}

sf::IntRect Explosion::TileBoundingBox()
{
	return sf::IntRect((int)pos.x, (int)pos.y, 128, 128);
}

void Explosion::setPosition(sf::Vector2f position)
{
	pos = position;
	tex2d.sprite.setPosition(sf::Vector2f(position.x, position.y));
}


sf::Vector2f Explosion::getPosition()
{
	return pos;
}

void Explosion::LoadResources()
{
	tex2d.Load("rd/explode.png");
}

void Explosion::Animator(float deltaTime)
{
	currentFrame += animationSpeed * deltaTime;

	currentFrame = boost::algorithm::clamp(currentFrame, 0, 16);

	if (currentFrame >= 16)
	{
		//currentFrame = 0;
		animationCompleted = true;
	}

	spriteRect = sf::IntRect((int)currentFrame * 128, 0, 128, 128);
	tex2d.sprite.setTextureRect(spriteRect);
}

void Explosion::Update(float deltaTime)
{
	Animator(deltaTime);

	tex2d.sprite.setPosition(pos);
}