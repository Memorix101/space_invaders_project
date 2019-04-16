#pragma once
#include "Texture2D.h"

namespace sf {
	class Event;
}

class Player
{
public:
	Player();
	~Player();

	bool dead;
	bool shoot;
	SDL_Rect TileBoundingBox();
	SDL_Rect getPosition();
	void setPosition(SDL_Rect position);
	void LoadResources(SDL_Renderer* renderer);
	void Input(float deltaTime);
	void Update(float deltaTime);
	Texture2D tex2d;

	bool move_left = false;
	bool move_right = false;
protected:	

	SDL_Rect pos;	
	//float PosX;
	const int speed = 250;
};

