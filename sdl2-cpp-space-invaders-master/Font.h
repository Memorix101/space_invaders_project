#pragma once
#include <SDL2/SDL_ttf.h>
#include <string>

class Font
{
public:
	Font(std::string file, int size);
	~Font();

	void setColour(int r, int g, int b);
	SDL_Surface* toSurface(std::string text);
	SDL_Surface* toSurface(char* text);
	void setPosition(int x, int y);
	SDL_Rect getPosition();

private:
	TTF_Font* ttf_font;
	SDL_Color ttf_colour = { 255, 255, 255 };
	SDL_Rect pos;
};

