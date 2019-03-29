#include "Enemy.h"
#include <boost/algorithm/clamp.hpp>

Enemy::Enemy()
{
	shootTimer = 0.0f;
	LoadResources();
	spriteRect = sf::IntRect(0, 0, 32, 32);
	tex2d.sprite.setTextureRect(spriteRect);
}

Enemy::Enemy(sf::Vector2f position, int rowid, float maxShootTime)
{
	startPos = position.x;
	rowPosID = 40 * (11 - rowid);
	shootTimer = 0.0f;
	shootTimeLimit = maxShootTime;
	LoadResources();
	spriteRect = sf::IntRect(0, 0, 32, 32);
	tex2d.sprite.setTextureRect(spriteRect);
	pos = position;
	tex2d.sprite.setPosition(sf::Vector2f(position.x, position.y));
}

Enemy& Enemy::operator=(const Enemy& other)
{
	startPos = other.startPos;
	rowPosID = other.rowPosID;
	shootTimer = other.shootTimer;
	shootTimeLimit = other.shootTimeLimit;
	pos = other.pos;
	return *this;
}

Enemy::~Enemy()
{
}

sf::IntRect Enemy::TileBoundingBox()
{
	return sf::IntRect((int)pos.x, (int)pos.y, 28, 21);
}

void Enemy::setPosition(sf::Vector2f position)
{
	pos = position;
	tex2d.sprite.setPosition(sf::Vector2f(position.x, position.y));
}


sf::Vector2f Enemy::getPosition()
{
	return pos;
}

void Enemy::LoadResources()
{
	tex2d.Load("rd/invader32x32x4.png");
}

void Enemy::Animator(float deltaTime)
{
	currentFrame += animationSpeed * deltaTime;

	currentFrame = boost::algorithm::clamp(currentFrame, 0, 4);

	if (currentFrame >= 4)
	{
		currentFrame = 0;
	}

	spriteRect = sf::IntRect((int)currentFrame * 32, 0, 32, 32);
	tex2d.sprite.setTextureRect(spriteRect);
}

void Enemy::Update(float deltaTime)
{
	Animator(deltaTime);

	if (!goLeft)
	{
		pos.x += moveSpeed * deltaTime;
	}

	if (goLeft)
	{
		pos.x -= moveSpeed * deltaTime;
	}

	if (pos.x >= 640 - (spriteRect.width + rowPosID) && !goLeft)
	{
		goLeft = true;
	}

	if (pos.x <= startPos + spriteRect.width && goLeft)
	{
		goLeft = false;
	}

	shootTimer += 1 * deltaTime;

	if (shootTimer >= shootTimeLimit)
	{
		shootTimer = 0.0f;
		shoot = true;
	}
	else
	{
		shoot = false;
	}

	tex2d.sprite.setPosition(pos);
}