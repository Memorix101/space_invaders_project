#include "Texture2D.h"


Texture2D::Texture2D()
{
}

Texture2D::~Texture2D()
{

}

void Texture2D::Load(std::string file, SDL_Renderer* renderer)
{
	surface = IMG_Load(file.c_str());
	texture = SDL_CreateTextureFromSurface(renderer, surface);
	bounds = {surface->w,surface->h};
	SDL_FreeSurface(surface);
}

void Texture2D::Dispose()
{
	SDL_DestroyTexture(texture);
}