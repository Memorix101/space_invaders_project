#include "SoundEffect.h"
#include <iostream>


SoundEffect::SoundEffect(std::string file)
{
	sound = Mix_LoadWAV(file.c_str());
}

SoundEffect::~SoundEffect()
{
}

void SoundEffect::Dispose()
{
	Mix_FreeChunk(sound);
}

void SoundEffect::Play()
{	
	Mix_PlayChannel(-1, sound, 0); //-1 for the first free unreserved channel.
}
