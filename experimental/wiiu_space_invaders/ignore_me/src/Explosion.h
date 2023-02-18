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
	SDL_Rect getSpriteCut();
	SDL_Rect getPosition();
	void Animator(float deltaTime);
	void Update(float deltaTime);
	SDL_Rect getSpriteRect();
	void setPosition(SDL_Rect position);
	Texture2D tex2d;
	void LoadResources(SDL_Renderer* renderer);
	bool animationCompleted = false;

protected:
	const int moveSpeed = 50;
	SDL_Rect pos;
	const float animationSpeed = 25.0f;
	float currentFrame;
	SDL_Rect spriteRect;
};

