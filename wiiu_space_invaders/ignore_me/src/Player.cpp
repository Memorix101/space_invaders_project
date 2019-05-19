#include "Player.h"
#include <SDL2/SDL.h>

Player::Player()
{
	shoot = false;
	dead = false;
}

Player::~Player()
{
}

SDL_Rect Player::TileBoundingBox()
{
	return { pos.x,pos.y,tex2d.bounds.width,tex2d.bounds.height };
}

SDL_Rect Player::getPosition()
{
	return pos;
}

void Player::setPosition(SDL_Rect position)
{
	pos = { position.x, position.y, 0, 0 };
}

void Player::LoadResources(SDL_Renderer* renderer)
{
	tex2d.Load("rd/player.png", renderer);
	pos.x = 640 / 2 - tex2d.bounds.width / 2;
}

void Player::Input(float deltaTime)
{
	const Uint8* currentKeyStates = SDL_GetKeyboardState(nullptr);

	//continuous-response keys
	if (currentKeyStates[SDL_SCANCODE_RIGHT] || currentKeyStates[SDL_SCANCODE_D] || move_right)
	{
		pos.x += speed * deltaTime;
	}
	else if (currentKeyStates[SDL_SCANCODE_LEFT] || currentKeyStates[SDL_SCANCODE_A] || move_left)
	{
		pos.x -= speed * deltaTime;
	}
}

void Player::Update(float deltaTime)
{
	if (!dead)
	{
		Input(deltaTime);
		//pos.x = boost::algorithm::clamp(pos.x, 0, 640 - tex2d.bounds.width);
		pos = { pos.x, (480 - 32) - tex2d.bounds.width / 2 };
	}
}