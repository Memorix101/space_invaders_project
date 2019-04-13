#include "Music.h"

Music::Music(std::string file)
{
	music = Mix_LoadMUS(file.c_str());
}

Music::~Music()
{
}

bool Music::Play(bool looping)
{	
	int _looping = 0; //0 plays the music zero times...
	
	if (looping)
		_looping = -1; //plays the music forever (or as close as it can get to that)

	if (Mix_PlayMusic(music, _looping) == -1) //Play the music
	{
		return 1;
	}
}
