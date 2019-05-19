#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include "utils.h"

class Texture2D
{
public:
	Texture2D();
	~Texture2D();

	SDL_Texture* texture;
	Bounds bounds;
	void Load(std::string file, SDL_Renderer* renderer);
	void Dispose();

private:
	SDL_Surface* surface;
};
