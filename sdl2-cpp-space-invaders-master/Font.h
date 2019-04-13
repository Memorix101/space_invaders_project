#pragma once
#include <SDL2/SDL_ttf.h>
#include <string>
#include "utils.h"
#include "Texture2D.h"

class Font
{
public:
	Font(std::string file, int size);
	~Font();
	void Dispose();

	void setColour(int r, int g, int b);
	SDL_Surface* toSurface(std::string text);
	SDL_Surface* toSurface(char* text);	
	SDL_Texture* toTexture(std::string text, SDL_Renderer* renderer);
	SDL_Rect TileBoundingBox();
	SDL_Texture* toTexture(char* text, SDL_Renderer* renderer);
	void setPosition(int x, int y);
	SDL_Rect getPosition();
	Bounds bounds;

private:
	TTF_Font* ttf_font;
	Texture2D tex2d;
	SDL_Color ttf_colour = { 255, 255, 255 };
	SDL_Rect pos;
};

