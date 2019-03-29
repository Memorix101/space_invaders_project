#pragma once
#include "Texture2D.h"

class Bullet
{
public:	
	Bullet();
	Bullet& operator=(const Bullet& other);
	~Bullet();

	bool reversed = false;
	sf::Vector2f pos = sf::Vector2f(0, 0);
	sf::IntRect TileBoundingBox();
	sf::Vector2f getPosition();
	void setPosition(sf::Vector2f position);
	void Update(float deltaTime);
	Texture2D tex2d;
	void LoadResources();

private:	
	const int speed = 250;
	sf::IntRect spriteRect;	
};

