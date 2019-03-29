#include "SoundEffect.h"
#include <iostream>


SoundEffect::SoundEffect(std::string file)
{
	if (!buffer.loadFromFile(file))
		std::cout << "Couldn't load sound file: " << file << std::endl;

	snd.setBuffer(buffer);
}

SoundEffect::~SoundEffect()
{
}

void SoundEffect::Play()
{	
	snd.play();
}
