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
		spriteRect = sf::IntRect(0, 0, tex2d.sprite.getGlobalBounds().width, tex2d.sprite.getGlobalBounds().height);
	}
	else
	{
		tex2d.Load("rd/enemy-bullet.png");
		spriteRect = sf::IntRect(0, 0, tex2d.sprite.getGlobalBounds().width, tex2d.sprite.getGlobalBounds().height);
	}
}

sf::IntRect Bullet::TileBoundingBox()
{
	return sf::IntRect((int)pos.x, (int)pos.y, 28, 21);
}

void Bullet::setPosition(sf::Vector2f position)
{
	pos = position;
	tex2d.sprite.setPosition(sf::Vector2f(position.x, position.y));
}

sf::Vector2f Bullet::getPosition()
{
	return pos;
}

void Bullet::Update(float deltaTime)
{
	if (reversed)
	{
		pos.y += 150 * deltaTime;
	}
	else
	{
		pos.y -= 150 * deltaTime;
	}

	tex2d.sprite.setPosition(pos);
}