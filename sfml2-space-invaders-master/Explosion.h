#pragma once
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Rect.hpp>
#include "Texture2D.h"

class Explosion
{
public:
	Explosion();
	Explosion(sf::Vector2f position, int rowid, int maxShootTime);
	Explosion& operator=(const Explosion& other);
	~Explosion();

	sf::IntRect TileBoundingBox();
	sf::Vector2f getPosition();
	void Animator(float deltaTime);
	void Update(float deltaTime);
	void setPosition(sf::Vector2f position);
	Texture2D tex2d;
	bool animationCompleted = false;
	void LoadResources();

protected:
	const int moveSpeed = 50;
	sf::Vector2f pos;
	sf::IntRect spriteRect;
	const float animationSpeed = 25.0f;
	float currentFrame;
};

