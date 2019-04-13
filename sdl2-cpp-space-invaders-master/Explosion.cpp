#include "Explosion.h"
#include "Enemy.h"
#include <boost/algorithm/clamp.hpp>


Explosion::Explosion()
{;
	LoadResources();
	spriteRect = { 0, 0, 128, 128 };
}

Explosion::Explosion(SDL_Rect position, int rowid, int maxShootTime)
{;
	LoadResources();
	spriteRect = { 0, 0, 128, 128 };
	pos = position;
}

Explosion& Explosion::operator=(const Explosion& other)
{
	pos = other.pos;
	return *this;
}

Explosion::~Explosion()
{
}

SDL_Rect Explosion::TileBoundingBox()
{
	return { pos.x, (int)pos.y, 128, 128 };
}

void Explosion::setPosition(SDL_Rect position)
{
	pos = position;
}


SDL_Rect Explosion::getPosition()
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

	spriteRect = { (int)currentFrame * 128, 0, 128, 128 };
}

void Explosion::Update(float deltaTime)
{
	Animator(deltaTime);
}