#pragma once

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The surface contained by the window
SDL_Surface* gScreenSurface = NULL;

//The image we will load and show on the screen
SDL_Surface* gHelloWorld = NULL;

//The image we will load and show on the screen
SDL_Surface* blueGuy = NULL;

//Event handler
SDL_Event e;

//Music
Mix_Music *music = NULL;

//Sound
Mix_Chunk *snd_powerup = NULL;

//Starts up SDL and creates window
bool init();

//Garbage collector when shuting down the application
void close();