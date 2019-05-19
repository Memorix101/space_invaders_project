#include "Enemy.h"

Enemy::Enemy()
{
	shootTimer = 0.0f;
}

Enemy::Enemy(SDL_Rect position, int rowid, float maxShootTime, SDL_Renderer* renderer)
{
	startPos = position.x;
	rowPosID = 40 * (11 - rowid);
	shootTimer = 0.0f;
	shootTimeLimit = maxShootTime;
	LoadResources(renderer);
	spriteRect = { 0, 0, 32, 32};
	pos = position;
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

SDL_Rect Enemy::TileBoundingBox()
{
	return {pos.x, pos.y,tex2d.bounds.width,tex2d.bounds.height};
}

SDL_Rect Enemy::getSpriteCut()
{
	return { pos.x, pos.y,spriteRect.w,spriteRect.h};
}

SDL_Rect Enemy::getSpriteRect()
{
	return { spriteRect.x, spriteRect.y,spriteRect.w,spriteRect.h};
}

void Enemy::setPosition(SDL_Rect position)
{
	pos = position;
}

SDL_Rect Enemy::getPosition()
{
	return pos;
}

void Enemy::LoadResources(SDL_Renderer* renderer)
{
	tex2d.Load("rd/invader32x32x4.png", renderer);
	spriteRect = { 0, 0, 32, 32};
}

void Enemy::Animator(float deltaTime)
{
	currentFrame += animationSpeed * deltaTime;

	//currentFrame = boost::algorithm::clamp(currentFrame, 0, 4);

	if (currentFrame >= 4)
	{
		currentFrame = 0;
	}

	spriteRect = {int(currentFrame) * 32, 0, 32, 32};
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
		pos.x -= moveSpeed * deltaTime * 0.75f; //maybe I'm turned nuts or something, but there are going faster here. decreasing by 25% fixes this. maybe someone can spot the issue here.
	}

	if (pos.x >= 640 - (spriteRect.w + rowPosID) && !goLeft)
	{
		goLeft = true;
	}

	if (pos.x <= startPos + spriteRect.w && goLeft)
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
}