#pragma once
#include "Texture2D.h"

class Enemy
{
public:
	Enemy();
	Enemy(SDL_Rect position, int rowid, float maxShootTime);
	Enemy& operator=(const Enemy& other);
	~Enemy();

	SDL_Rect TileBoundingBox();
	SDL_Rect getPosition();

	void Animator(float deltaTime);
	void Update(float deltaTime);

	float startPos;
	int rowPosID;
	float shootTimeLimit;
	bool shoot = false;

	void setPosition(SDL_Rect position);
	Texture2D tex2d;
	SDL_Rect spriteRect;

protected:
	void LoadResources();
	const int moveSpeed = 75;
	SDL_Rect pos;
	const float animationSpeed = 25.0f;
	float currentFrame;
	bool goLeft = false;
	float shootTimer;
	const int speed = 250;
};

