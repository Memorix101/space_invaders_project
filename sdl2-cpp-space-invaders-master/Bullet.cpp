#include "Bullet.h"

Bullet::Bullet()
{

}

Bullet& Bullet::operator=(const Bullet& other)
{
	reversed = other.reversed;
	tex2d = other.tex2d;
	spriteRect = other.spriteRect;
	pos = other.pos;
	return *this;
}

Bullet::~Bullet()
{
}

void Bullet::LoadResources()
{
	if (!reversed)
	{
		tex2d.Load("rd/bullet.png");
		spriteRect = { 0, 0, tex2d.sprite->w, tex2d.sprite->h };
	}
	else
	{
		tex2d.Load("rd/enemy-bullet.png");
		spriteRect = { 0, 0, tex2d.sprite->w, tex2d.sprite->h };
	}
}

SDL_Rect Bullet::TileBoundingBox()
{
	return { pos.x, pos.y, 28, 21};
}

void Bullet::setPosition(SDL_Rect position)
{
	pos = position;
}

SDL_Rect Bullet::getPosition()
{
	return pos;
}

void Bullet::Update(float deltaTime)
{
	if (reversed)
	{
		pos.y += speed * deltaTime;
	}
	else
	{
		pos.y -= speed * deltaTime;
	}
}