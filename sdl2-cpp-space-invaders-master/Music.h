#pragma once
#include <SDL2/SDL_mixer.h>
#include <string>

class Music
{
public:
	Music(std::string file);
	~Music();

	Mix_Music* music;

	void Load(std::string file);
	bool Play(bool looping);
};

