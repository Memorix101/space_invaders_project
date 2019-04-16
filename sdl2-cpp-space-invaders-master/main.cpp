#include <iostream>
#include <stdlib.h>     /* srand, rand */
#include <sstream>      // std::stringstream
#include <SDL2/SDL.h>
#undef main
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include "Texture2D.h"
#include "Player.h"
#include "Enemy.h"
#include <iomanip>
#include "Bullet.h"
#include <list>
#include "Explosion.h"
#include "SoundEffect.h"
#include "Music.h"
#include "Font.h"

SDL_Event e;
SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;

SDL_GameController* controller = nullptr;
SDL_Haptic* haptic = nullptr;
#define GAMEPAD_DEADZONE  32768.0f
float LX = 0;

float deltaTime = 0.0f;
int score = 0;
int rowCount = 0;
int itemCount = 0;
bool gamestart = false;
float timer = 0.0f;
float lastTick = 0.0f;
bool gameover = false;
bool quit = false;
std::stringstream buffer; //score buffer

Player player;
std::list<Bullet> laser;
Bullet _laser;
std::list<Bullet> beam;
Bullet _beam;
std::list<Enemy> enemy;
Enemy _enemy;
std::list<Explosion> explo_fx;
Explosion _explo_fx;

Texture2D fmg_logo;
Texture2D win_tex2d;
Texture2D gameover_tex2D;
Texture2D background;

void initEnemies()
{
	// create enemies
	for (int i = 0; i < 40; i++)
	{
		if (i % 10 == 0)
		{
			itemCount = 0;
			rowCount++;
		}

		itemCount++;

		int ran = (rand() % (20 - 3)) + 3; // MAX - MIN + MIN
		_enemy.LoadResources(renderer);
		_enemy.setPosition({ itemCount * (int)40.0f, (int)40.0f * rowCount });
		_enemy.startPos = itemCount * 40;
		_enemy.rowPosID = 40 * (11 - itemCount);
		_enemy.shootTimeLimit = ran;
		enemy.push_back(_enemy);

		//std::cout << "row:" << rowCount << std::endl;
		//std::cout << "item:" << itemCount * 40 << std::endl;
	}
}

void reset()
{
	gameover = false;
	score = 0;
	enemy.clear();
	beam.clear();
	laser.clear();
	explo_fx.clear();
	rowCount = 0;
	itemCount = 0;
	initEnemies();
	player.setPosition({ 640 / 2 - player.tex2d.bounds.width / 2 });
	player.dead = false;
}

int main()
{
	std::cout << "SDL2 C++ Space Invaders Game" << std::endl;

	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER | SDL_INIT_HAPTIC);
	SDL_ShowCursor(SDL_DISABLE);
	window = SDL_CreateWindow("sdl2_cpp_space_invaders", SDL_WINDOWPOS_CENTERED, 
		SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	//Initialize SDL_mixer
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) == -1)
	{
		return false;
	}

	//Set up TTf stuff
	if (TTF_Init() == -1)
	{
		return false;
	}

	//Get the first available controller
	for (int i = 0; i < SDL_NumJoysticks(); ++i) {
		if (SDL_IsGameController(i)) {
			controller = SDL_GameControllerOpen(i);
			if (controller) {
				const char* name = SDL_GameControllerNameForIndex(i);
				if (name)
					printf("Controller %i has game controller name '%s'\n", i, name);

				// Open the device
				haptic = SDL_HapticOpen(0);
				if (haptic == nullptr)
					return -1;

				// Initialize simple rumble
				if (SDL_HapticRumbleInit(haptic) != 0)
					return -1;
				break;
			}
			else {
				fprintf(stderr, "Could not open Controller %i: %s\n", i, SDL_GetError());
			}
		}
	}

	fmg_logo.Load("rd/fmg_splash.png", renderer);
	win_tex2d.Load("rd/win_ui.png", renderer);
	gameover_tex2D.Load("rd/gameover_ui.png", renderer);
	background.Load("rd/space3.png", renderer);

	player.LoadResources(renderer);
	_laser.LoadResources(renderer);
	_beam.reversed = true;
	_beam.LoadResources(renderer);

	Texture2D scoreText;
	Font vermin_ttf("rd/vermin_vibes_1989.ttf", 24);

	SoundEffect snd_blaster("rd/blaster.ogg");
	SoundEffect snd_blasterEnemy("rd/pusher.ogg");
	SoundEffect snd_explo("rd/explode1.ogg");

	Music music("rd/bodenstaendig.ogg");
	music.Play(true);

	initEnemies();

	while (!quit)
	{
		deltaTime = float(SDL_GetTicks()) / 1000.0f - lastTick;

		while (SDL_PollEvent(&e) != 0)
		{
			switch (e.type) {
			case SDL_KEYDOWN:
				if (e.key.keysym.sym == SDLK_SPACE && !player.dead && gamestart)
				{
					player.shoot = true;
					_laser.setPosition({ player.getPosition().x + player.tex2d.bounds.width / 2 - 2, 
						player.getPosition().y - 32 });//1.5f is half of laser width ;)
					laser.push_back(_laser);
					snd_blaster.Play();
				}

				if (e.key.keysym.sym == SDLK_ESCAPE)
				{
					quit = true;
				}

				if (e.key.keysym.sym == SDLK_RETURN && gameover == 1)
				{
					reset();
				}
				break;

			case SDL_CONTROLLERBUTTONDOWN:
				if (e.type == SDL_CONTROLLERBUTTONDOWN && e.cbutton.button == SDL_CONTROLLER_BUTTON_A && !player.dead && gamestart)
				{
					player.shoot = true;
					_laser.setPosition({ player.getPosition().x + player.tex2d.bounds.width / 2 - 2, 
						player.getPosition().y - 32 });//1.5f is half of laser width ;)
					laser.push_back(_laser);
					snd_blaster.Play();
				}

				if (e.type == SDL_CONTROLLERBUTTONDOWN && e.cbutton.button == SDL_CONTROLLER_BUTTON_BACK)
				{
					quit = true;
				}

				if (e.type == SDL_CONTROLLERBUTTONDOWN && e.cbutton.button == SDL_CONTROLLER_BUTTON_START && gameover == 1)
				{
					reset();
				}

				if (e.type == SDL_CONTROLLERBUTTONDOWN && e.cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_RIGHT)
				{
					player.move_right = true;
				}
				else if (e.type == SDL_CONTROLLERBUTTONDOWN && e.cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_LEFT)
				{
					player.move_left = true;
				}
				break;

			case  SDL_CONTROLLERBUTTONUP:

				//continuous-response keys
				if (e.type == SDL_CONTROLLERBUTTONUP && e.cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_RIGHT)
				{
					player.move_right = false;
				}
				else if (e.type == SDL_CONTROLLERBUTTONUP && e.cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_LEFT)
				{
					player.move_left = false;
				}
				break;

			case SDL_CONTROLLERAXISMOTION:  //https://docs.microsoft.com/en-gb/windows/desktop/xinput/getting-started-with-xinput#dead_zone

				// handle axis motion
				if (e.caxis.axis == SDL_CONTROLLER_AXIS_LEFTX)
				{
					LX = float(e.caxis.value) / GAMEPAD_DEADZONE;
					//printf("LX: %f\n", (float)LX);
				}

				if (LX <= -0.5f)
				{
					player.move_right = false;
					player.move_left = true;
				}

				if (LX >= 0.5f)
				{
					player.move_left = false;
					player.move_right = true;
				}

				if (LX < 0.5f && LX > -0.5f)
				{
					player.move_left = false;
					player.move_right = false;
				}
				break;
			}
		}

		timer += deltaTime;

		if (timer >= 5.0f)
			gamestart = true;

		//std::cout << timer << std::endl;

		if (gamestart) {

			player.Input(deltaTime);
			player.Update(deltaTime);

			for (auto it = enemy.begin(); it != enemy.end(); it++) {

				if (it->shoot)
				{
					_beam.reversed = true;
					_beam.setPosition({ it->getPosition().x + it->getSpriteRect().w * 1 - 4, 
						it->getPosition().y + 9 }); //1.5f is half of laser width ;)
					beam.push_back(_beam);
					snd_blasterEnemy.Play();
				}

				for (auto itb = laser.begin(); itb != laser.end(); itb++)
				{
					if (SDL_HasIntersection(&it->getSpriteCut(), &itb->TileBoundingBox()))
					{
						score += 100;
						_explo_fx.LoadResources(renderer);
						_explo_fx.setPosition({ it->getPosition().x - _explo_fx.getSpriteCut().w / 2 + it->getSpriteCut().w / 2, 
							it->getPosition().y - _explo_fx.getSpriteCut().h / 2 + it->getSpriteCut().h / 2 });
						explo_fx.push_back(_explo_fx);
						laser.erase(itb);
						enemy.erase(it);
						snd_explo.Play();
						goto escapeFromHere;
					}
				}
			}

		escapeFromHere:

			for (auto it = enemy.begin(); it != enemy.end(); it++)
				it->Update(deltaTime);

			for (auto it = beam.begin(); it != beam.end(); it++)
			{
				if (SDL_HasIntersection(&it->TileBoundingBox(), &player.TileBoundingBox()) && !player.dead)
				{
					_explo_fx.LoadResources(renderer);
					_explo_fx.setPosition({ it->getPosition().x - 128 / 2, it->getPosition().y - 128 / 2 });
					explo_fx.push_back(_explo_fx);
					player.dead = true;
					snd_explo.Play();
					beam.erase(it);
					// Play effect at 50% strength for 2 seconds
					SDL_HapticRumblePlay(haptic, 0.5, 500);
					break;
				}

				if (it->getPosition().y >= 480)
				{
					beam.erase(it);
					break;
				}
			}

			for (auto it = beam.begin(); it != beam.end(); it++)
				it->Update(deltaTime);

			for (auto it = laser.begin(); it != laser.end(); it++)
			{
				it->Update(deltaTime);

				if (it->getPosition().y <= 0)
				{
					laser.erase(it);
					break;
				}
			}

			for (auto it = explo_fx.begin(); it != explo_fx.end(); it++)
			{
				it->Update(deltaTime);
				if (it->animationCompleted)
				{
					explo_fx.erase(it);
					break;
				}
			}
		}

		//draw loop
		SDL_RenderClear(renderer);

		if (gamestart) {

			SDL_RenderCopy(renderer, background.texture, nullptr, nullptr);

			if (!player.dead)
				SDL_RenderCopy(renderer, player.tex2d.texture, nullptr, &player.TileBoundingBox());

			for (Bullet l : laser)
				SDL_RenderCopy(renderer, l.tex2d.texture, nullptr, &l.TileBoundingBox());

			for (Bullet b : beam)
				SDL_RenderCopy(renderer, b.tex2d.texture, nullptr, &b.TileBoundingBox());

			for (Explosion ex : explo_fx)
				SDL_RenderCopy(renderer, ex.tex2d.texture, &ex.getSpriteRect(), &ex.getSpriteCut());

			for (Enemy e : enemy)
				SDL_RenderCopy(renderer, e.tex2d.texture, &e.getSpriteRect(), &e.getSpriteCut());

			if (enemy.empty())
			{
				gameover = true;
				SDL_RenderCopy(renderer, win_tex2d.texture, nullptr, nullptr);
			}
			else if (player.dead)
			{
				gameover = true;
				SDL_RenderCopy(renderer, gameover_tex2D.texture, nullptr, nullptr);
			}

			buffer << "SCORE:" << std::setw(4) << std::setfill('0') << score;
			scoreText.texture = vermin_ttf.toTexture(buffer.str().c_str(), renderer);
			vermin_ttf.setPosition(640 - vermin_ttf.bounds.width - 10, 10);
			SDL_RenderCopy(renderer, scoreText.texture, nullptr, &vermin_ttf.TileBoundingBox());
			buffer.str(""); // empty string
		}
		else
		{
			SDL_RenderCopy(renderer, fmg_logo.texture, nullptr, nullptr);
		}

		SDL_RenderPresent(renderer);
		lastTick = float(SDL_GetTicks()) / 1000.0f;
		SDL_Delay(15);
	}

	//cleaning up stuff
	SDL_GameControllerClose(controller);
	SDL_HapticClose(haptic);
	vermin_ttf.Dispose();
	fmg_logo.Dispose();
	background.Dispose();
	player.tex2d.Dispose();
	music.Dispose();
	snd_blaster.Dispose();
	snd_explo.Dispose();
	snd_blasterEnemy.Dispose();
	enemy.clear();
	laser.clear();
	explo_fx.clear();
	beam.clear();
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
