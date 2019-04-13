#include "Font.h"

Font::Font(std::string file, int size)
{
	ttf_font = TTF_OpenFont(file.c_str(), size);
}

void Font::setColour(int r, int g, int b)
{
	ttf_colour = { (Uint8)r, (Uint8)g, (Uint8)b };
}

SDL_Surface* Font::toSurface(std::string text)
{
	return TTF_RenderText_Solid(ttf_font, text.c_str(), ttf_colour);
}

SDL_Surface* Font::toSurface(char* text)
{
	return TTF_RenderText_Solid(ttf_font, text, ttf_colour);
}

void Font::setPosition(int x, int y)
{
	pos = { x, y, 0, 0 };
}

SDL_Rect Font::getPosition()
{
	return pos;
}

Font::~Font()
{
}
