#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>

class Texture2D
{
public:
	Texture2D();
	~Texture2D();

	SDL_Surface* sprite;

	void Load(std::string file);
};