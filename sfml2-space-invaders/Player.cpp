#include "Player.h"
#include <SFML/Window/Keyboard.hpp>
#include <boost/algorithm/clamp.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

Player::Player()
{
	shoot = false;
	dead = false;
}

Player::~Player()
{
}

sf::IntRect Player::TileBoundingBox()
{
	return sf::IntRect((int)pos.x, (int)pos.y, 28, 21);
}

sf::Vector2f Player::getPosition()
{
	return pos;
}

void Player::setPosition(sf::Vector2f position)
{
	pos = position;
	tex2d.sprite.setPosition(position);
}

void Player::LoadResources()
{
	tex2d.Load("rd/player.png");
	pos.x = 640 / 2 - tex2d.sprite.getGlobalBounds().width / 2;
}

void Player::Input(float deltaTime)
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D))
	{
		pos.x += speed * deltaTime;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A))
	{
		pos.x -= speed * deltaTime;
	}
}

void Player::InputEvent(bool e)
{
	e = true;	
}

void Player::Update(float deltaTime)
{
	if (!dead)
	{
		Input(deltaTime);
		pos.x = boost::algorithm::clamp(pos.x, 0, 640 - tex2d.sprite.getGlobalBounds().width);
		pos = sf::Vector2f(pos.x, (480 - 30) - tex2d.sprite.getGlobalBounds().width / 2);
		tex2d.sprite.setPosition(pos);
	}
}