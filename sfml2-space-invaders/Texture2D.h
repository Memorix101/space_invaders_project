#pragma once
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <iostream>

class Texture2D
{
public:
	Texture2D();
	~Texture2D();

	sf::Sprite sprite;
	sf::Texture texture;

	void Load(std::string file);
};