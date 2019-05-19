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

SDL_Texture* Font::toTexture(char* text, SDL_Renderer* renderer)
{
	SDL_Surface* surface = TTF_RenderText_Solid(ttf_font, text, ttf_colour);
	tex2d.texture = SDL_CreateTextureFromSurface(renderer, surface);
	tex2d.bounds = { surface->w, surface->h };
	bounds = { surface->w, surface->h };
	SDL_FreeSurface(surface);
	return tex2d.texture;
}

SDL_Texture* Font::toTexture(std::string text, SDL_Renderer* renderer)
{
	SDL_Surface* surface = TTF_RenderText_Solid(ttf_font, text.c_str(), ttf_colour);
	tex2d.texture = SDL_CreateTextureFromSurface(renderer, surface);
	tex2d.bounds = { (int)surface->w, (int)surface->h };
	bounds = { (int)surface->w, (int)surface->h };
	SDL_FreeSurface(surface);
	return tex2d.texture;
}

SDL_Rect Font::TileBoundingBox()
{
	return { pos.x,pos.y,tex2d.bounds.width,tex2d.bounds.height };
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


void Font::Dispose()
{
	TTF_CloseFont(ttf_font);
}