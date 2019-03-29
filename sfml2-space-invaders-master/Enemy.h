#pragma once
#include "Texture2D.h"

class Enemy
{
public:
	Enemy();
	Enemy(sf::Vector2f position, int rowid, float maxShootTime);
	Enemy& operator=(const Enemy& other);
	~Enemy();

	sf::IntRect TileBoundingBox();
	sf::Vector2f getPosition();

	void Animator(float deltaTime);
	void Update(float deltaTime);

	float startPos;
	int rowPosID;
	float shootTimeLimit;
	bool shoot = false;

	void setPosition(sf::Vector2f position);
	Texture2D tex2d;

protected:
	void LoadResources();
	const int moveSpeed = 75;
	sf::Vector2f pos;
	sf::IntRect spriteRect;
	const float animationSpeed = 25.0f;
	float currentFrame;
	bool goLeft = false;
	float shootTimer;
	const int speed = 250;
};

