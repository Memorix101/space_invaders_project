#include "Explosion.h"
#include "Enemy.h"
#include <boost/algorithm/clamp.hpp>


Explosion::Explosion()
{;

}

Explosion::Explosion(SDL_Rect position, int rowid, int maxShootTime)
{;
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
	return { pos.x, pos.y,tex2d.bounds.width,tex2d.bounds.height };
}

SDL_Rect Explosion::getSpriteCut()
{
	return { pos.x, pos.y,spriteRect.w,spriteRect.h };
}

SDL_Rect Explosion::getSpriteRect()
{
	return { spriteRect.x, spriteRect.y,spriteRect.w,spriteRect.h };
}

void Explosion::setPosition(SDL_Rect position)
{
	pos = position;
}


SDL_Rect Explosion::getPosition()
{
	return pos;
}

void Explosion::LoadResources(SDL_Renderer* renderer)
{
	tex2d.Load("rd/explode.png", renderer);
	spriteRect = { 0, 0, 128, 128};
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

	spriteRect = { int(currentFrame) * 128, 0,128, 128};
}

void Explosion::Update(float deltaTime)
{
	Animator(deltaTime);
}