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
	void InputEvent(bool e);
	sf::IntRect TileBoundingBox();
	sf::Vector2f getPosition();
	void setPosition(sf::Vector2f position);
	void LoadResources();
	void Input(float deltaTime);
	void Update(float deltaTime);
	Texture2D tex2d;
protected:	

	sf::Vector2f pos;
	const int speed = 250;
};

