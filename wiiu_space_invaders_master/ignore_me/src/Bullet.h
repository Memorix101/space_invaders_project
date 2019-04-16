#pragma once
#include "Texture2D.h"
#include <SDL2/SDL.h>

class Bullet
{
public:	
	Bullet();
	Bullet& operator=(const Bullet& other);
	~Bullet();

	bool reversed = false;
	SDL_Rect pos = { 0, 0, 0, 0 };
	SDL_Rect TileBoundingBox();
	SDL_Rect getPosition();
	void setPosition(SDL_Rect position);
	void Update(float deltaTime);
	Texture2D tex2d;
	void LoadResources(SDL_Renderer* rendere);

private:	
	const int speed = 300;
	SDL_Rect spriteRect;
};

