#pragma once
#include <SDL2/SDL.h>
#include "Texture2D.h"

class Explosion
{
public:
	Explosion();
	Explosion(SDL_Rect position, int rowid, int maxShootTime);
	Explosion& operator=(const Explosion& other);
	~Explosion();

	SDL_Rect TileBoundingBox();
	SDL_Rect getPosition();
	void Animator(float deltaTime);
	void Update(float deltaTime);
	void setPosition(SDL_Rect position);
	Texture2D tex2d;
	bool animationCompleted = false;
	SDL_Rect spriteRect;

protected:
	void LoadResources();
	const int moveSpeed = 50;
	SDL_Rect pos;
	const float animationSpeed = 25.0f;
	float currentFrame;
};

