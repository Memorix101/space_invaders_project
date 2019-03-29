#pragma once
#include <SFML/Audio.hpp>

class SoundEffect
{
public:
	SoundEffect(std::string file);
	~SoundEffect();

	sf::SoundBuffer buffer;
	sf::Sound snd;

	void Load(std::string file);
	void Play();
};

