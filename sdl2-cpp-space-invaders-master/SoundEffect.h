#pragma once
#include <SDL2/SDL_mixer.h>
#include <string>

class SoundEffect
{
public:
	SoundEffect(std::string file);
	~SoundEffect();
	void Dispose();

	Mix_Chunk* sound;

	void Load(std::string file);
	void Play();
};

