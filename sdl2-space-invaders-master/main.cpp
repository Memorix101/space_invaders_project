#include <cstdlib>
#include <stdio.h>
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

struct player_t
{
	SDL_Rect hitbox;
	SDL_Rect pos;
	SDL_Texture* tex;
	int speed;
	SDL_Rect size;
	SDL_Rect vec; //representing pos and size
	int alive; //bool
};

struct bullet_t
{
	SDL_Rect hitbox;
	SDL_Rect pos;
	SDL_Texture* tex;
	int speed;
	SDL_Rect size;
	SDL_Rect vec; //representing pos and size
};

struct enemy_bullet_t
{
	SDL_Rect hitbox;
	SDL_Rect pos;
	SDL_Texture* tex;
	int speed;
	SDL_Rect size;
	SDL_Rect vec; //representing pos and size
};

struct explo_t
{
	SDL_Rect pos;
	SDL_Rect rect;
	SDL_Texture* tex;
	SDL_Rect size;
	SDL_Rect vec; //representing pos and size
	int animationFinished; //bool
	float currentFrame_ex;
	SDL_Rect spritecut;
};

struct enemy_t {
	SDL_Rect hitbox;
	SDL_Rect pos;
	SDL_Rect rect;
	SDL_Texture* tex;
	int goLeft; //bool
	int startPos;
	int rowPosID;
	int alive; //bool
	float shootTimer;
	int shoot; //bool
	float shootTimeLimit;
	int speed;
	SDL_Rect size;
	SDL_Rect vec; //representing pos and size
	SDL_Rect spritecut;
};

//base SDL stuff
SDL_Event e;
SDL_Renderer* renderer = NULL;
SDL_Window* window = NULL;

SDL_GameController* controller = NULL;
SDL_Haptic* haptic;
#define GAMEPAD_DEADZONE  32768.0f
float LX = 0;

//player
#define MAX_BULLETS 50
struct player_t player;

//enemy
#define MAX_ENEMY_BULLETS 100
#define MAX_ENEMIES 40
struct enemy_t* enemy[MAX_ENEMIES] = { NULL };
int enemies_killed = MAX_ENEMIES;
float currentFrame = 0.0f;

//stuff
struct bullet_t* bullets[MAX_BULLETS] = { NULL };
struct enemy_bullet_t* enemy_bullets[MAX_ENEMY_BULLETS] = { NULL };
#define MAX_EXPLO 100
struct explo_t* explo[MAX_EXPLO] = { NULL };

int rowCount = 0;
int itemCount = 0;
float deltaTime;
float lastTick;
int score = 0;
int gameover = 0;
int quit = 0;
bool move_left = 0;
bool move_right = 0;

//Music
Mix_Music* music = NULL;
Mix_Chunk* snd_pusher = NULL;
Mix_Chunk* snd_blaster = NULL;
Mix_Chunk* snd_explo = NULL;

//sprites and stuff
SDL_Surface* space3_surface;
SDL_Texture* space3_tex;
SDL_Surface* enemy_surface;
SDL_Texture* enemy_tex;
SDL_Surface* player_surface;
SDL_Texture* player_tex;
SDL_Surface* bullet_surface;
SDL_Texture* bullet_tex;
SDL_Surface* enemy_bullet_surface;
SDL_Texture* enemy_bullet_tex;
SDL_Surface* explo_surface;
SDL_Texture* explo_tex;
SDL_Surface* fmg_splash_surface;
SDL_Texture* fmg_splash_tex;
SDL_Surface* gameover_surface;
SDL_Texture* gameover_tex;
SDL_Surface* win_surface;
SDL_Texture* win_tex;
SDL_Texture* font_tex;

//Font stuff
TTF_Font* vermin_ttf;
SDL_Surface* scoreText;
SDL_Rect score_pos;
SDL_Rect score_tex_pos;

void load_assets()
{
	space3_surface = IMG_Load("rd/space3.png");
	space3_tex = SDL_CreateTextureFromSurface(renderer, space3_surface);
	SDL_FreeSurface(space3_surface);

	enemy_surface = IMG_Load("rd/invader32x32x4.png");
	enemy_tex = SDL_CreateTextureFromSurface(renderer, enemy_surface);

	player_surface = IMG_Load("rd/player.png");
	player_tex = SDL_CreateTextureFromSurface(renderer, player_surface);

	bullet_surface = IMG_Load("rd/bullet.png");
	bullet_tex = SDL_CreateTextureFromSurface(renderer, bullet_surface);

	enemy_bullet_surface = IMG_Load("rd/enemy-bullet.png");
	enemy_bullet_tex = SDL_CreateTextureFromSurface(renderer, enemy_bullet_surface);

	explo_surface = IMG_Load("rd/explode.png");
	explo_tex = SDL_CreateTextureFromSurface(renderer, explo_surface);

	fmg_splash_surface = IMG_Load("rd/fmg_splash.png");
	fmg_splash_tex = SDL_CreateTextureFromSurface(renderer, fmg_splash_surface);
	SDL_FreeSurface(fmg_splash_surface);

	gameover_surface = IMG_Load("rd/gameover_ui.png");
	gameover_tex = SDL_CreateTextureFromSurface(renderer, gameover_surface);
	SDL_FreeSurface(gameover_surface);

	win_surface = IMG_Load("rd/win_ui.png");
	win_tex = SDL_CreateTextureFromSurface(renderer, win_surface);
	SDL_FreeSurface(win_surface);
}

//Enemy Bullets
void addEnemyBullet(float x, float y)
{
	int found = -1;
	for (int i = 0; i < MAX_ENEMY_BULLETS; i++)
	{
		if (enemy_bullets[i] == NULL)
		{
			found = i;
			break;
		}
	}

	if (found >= 0)
	{
		int i = found;
		enemy_bullets[i] = (enemy_bullet_t*)malloc(sizeof(enemy_bullet_t));
		enemy_bullets[i]->tex = enemy_bullet_tex;
		enemy_bullets[i]->speed = 350;
		enemy_bullets[i]->pos = { (int)x, (int)y };
		enemy_bullets[i]->size = { 0, 0, enemy_bullet_surface->w, enemy_bullet_surface->h };
		enemy_bullets[i]->hitbox = { 0, 0, enemy_bullets[i]->size.w, enemy_bullets[i]->size.h };
	}
}

void removeEnemyBullet(int i)
{
	if (enemy_bullets[i])
	{
		free(enemy_bullets[i]);
		enemy_bullets[i] = NULL;
	}
}

void updateEnemyBullet()
{
	for (int i = 0; i < MAX_ENEMY_BULLETS; i++) if (enemy_bullets[i])
	{
		enemy_bullets[i]->pos.y += enemy_bullets[i]->speed * deltaTime;
		enemy_bullets[i]->vec = { enemy_bullets[i]->pos.x, enemy_bullets[i]->pos.y, enemy_bullets[i]->size.w, enemy_bullets[i]->size.h };

		if (enemy_bullets[i]->pos.y >= 480 - 9)
		{
			removeEnemyBullet(i);
		}
	}
}

void drawEnemyBullet()
{
	for (int i = 0; i < MAX_ENEMY_BULLETS; i++) if (enemy_bullets[i])
	{
		SDL_RenderCopy(renderer, enemy_bullets[i]->tex, NULL, &enemy_bullets[i]->vec);
	}
}

//Enemies
void initEnemies()
{
	for (int i = 0; i < MAX_ENEMIES; i++)
	{
		if (i % 10 == 0)
		{
			itemCount = 0;
			rowCount++;
		}
		itemCount++;
		
		int found = -1;
		for (int i = 0; i < MAX_ENEMIES; i++)
		{
			if (enemy[i] == NULL)
			{
				found = i;
				break;
			}
		}

		if (found >= 0)
		{
			int i = found;
			enemy[i] = (enemy_t*)malloc(sizeof(enemy)); //visual studio needs that "stupid" cast to operate >_>
			enemy[i]->tex = enemy_tex;
			enemy[i]->alive = 1;
			enemy[i]->rect = { 0, 0, 32, 32 };
			enemy[i]->pos = { itemCount * 40, 40 * rowCount };
			enemy[i]->startPos = enemy[i]->pos.x;
			enemy[i]->rowPosID = 40 * (11 - itemCount);
			enemy[i]->speed = 1; //bug: weird bug makes the enemies spawn out side of the viewport if speed is initialized here
			enemy[i]->goLeft = 0;
			enemy[i]->hitbox = { enemy[i]->pos.x, enemy[i]->pos.y, enemy[i]->rect.w, enemy[i]->rect.h };
			enemy[i]->shoot = 0;
			enemy[i]->shootTimer = 0;
			enemy[i]->shootTimeLimit = (rand() % (20 - 3)) + 3; //MAX - MIN + MIN
			enemy[i]->vec = { enemy[i]->pos.x, enemy[i]->pos.y, enemy[i]->size.w, enemy[i]->size.h };
			enemy[i]->spritecut = { enemy[i]->pos.x, enemy[i]->pos.y, enemy[i]->rect.w, enemy[i]->rect.h };
		}
	}
}

void animatorEnemies()
{
	currentFrame += 15.0f * deltaTime;

	if ((int)currentFrame >= 4)
	{
		currentFrame = 0;
	}

	for (int e = 0; e < MAX_ENEMIES; e++) if (enemy[e])
	{
		enemy[e]->rect.x = (int)currentFrame * 32;
	}
}

void updateEnemies()
{
	animatorEnemies();

	for (int e = 0; e < MAX_ENEMIES; e++) if (enemy[e])
	{
		if (enemy[e]->goLeft == 0)
		{
			enemy[e]->pos.x += enemy[e]->speed * deltaTime;
			enemy[e]->speed = 150; //bug: an weird bug makes the enemies spawn out side of the viewport if speed is initialized before this point
		}

		if (enemy[e]->goLeft == 1)
		{
			enemy[e]->pos.x -= enemy[e]->speed * deltaTime;
		}

		if (enemy[e]->pos.x >= 640 - (enemy[e]->rect.w + enemy[e]->rowPosID) && enemy[e]->goLeft == 0)
		{
			enemy[e]->goLeft = 1;
		}

		if (enemy[e]->pos.x <= enemy[e]->startPos + enemy[e]->rect.w && enemy[e]->goLeft == 1)
		{
			enemy[e]->goLeft = 0;
		}

		enemy[e]->shootTimer += 1 * deltaTime;

		if (enemy[e]->shootTimer >= enemy[e]->shootTimeLimit)
		{
			enemy[e]->shootTimer = 0;
			enemy[e]->shoot = 1;
		}
		else
		{
			enemy[e]->shoot = 0;
		}

		if (enemy[e]->shoot == 1 && enemy[e]->alive == 1)
		{
			addEnemyBullet(enemy[e]->pos.x + enemy[e]->rect.w / 2 - 4, enemy[e]->pos.y - 4);
			Mix_PlayChannel(2, snd_pusher, 0);
		}

		enemy[e]->hitbox = { enemy[e]->pos.x, enemy[e]->pos.y };
		enemy[e]->vec = { enemy[e]->pos.x, enemy[e]->pos.y, enemy[e]->size.w, enemy[e]->size.h };
		enemy[e]->spritecut = { enemy[e]->pos.x, enemy[e]->pos.y, enemy[e]->rect.w, enemy[e]->rect.h };
	}
}

void drawEnemies()
{
	for (int e = 0; e < MAX_ENEMIES; e++) if (enemy[e])
	{
		if (enemy[e]->alive == 1)
		{
			SDL_RenderCopy(renderer, enemy[e]->tex, &enemy[e]->rect, &enemy[e]->spritecut);
		}
	}
}

void removeEnemy(int i)
{
	if (enemy[i])
	{
		free(enemy[i]);
		enemy[i] = NULL;
	}
}

//Bullet
void addBullet(float x, float y)
{
	int found = -1;
	for (int i = 0; i < MAX_BULLETS; i++)
	{
		if (bullets[i] == NULL)
		{
			found = i;
			break;
		}
	}

	if (found >= 0)
	{
		int i = found;
		bullets[i] = (bullet_t*)malloc(sizeof(bullet_t));
		bullets[i]->tex = bullet_tex;
		bullets[i]->speed = 350;
		bullets[i]->pos = { (int)x, (int)y };
		bullets[i]->size = { 0, 0, bullet_surface->w, bullet_surface->h };
		bullets[i]->hitbox = { 0, 0, bullets[i]->size.w, bullets[i]->size.h };
	}
}

void removeBullet(int i)
{
	if (bullets[i])
	{
		free(bullets[i]);
		bullets[i] = NULL;
	}
}

void updateBullet()
{
	for (int i = 0; i < MAX_BULLETS; i++) if (bullets[i])
	{
		bullets[i]->pos.y -= bullets[i]->speed * deltaTime;
		bullets[i]->vec = { bullets[i]->pos.x, bullets[i]->pos.y, bullets[i]->size.w, bullets[i]->size.h };

		if (bullets[i]->pos.y <= 0)
		{
			removeBullet(i);
		}
	}
}

void drawBullet()
{
	for (int i = 0; i < MAX_BULLETS; i++) if (bullets[i])
	{
		SDL_RenderCopy(renderer, bullets[i]->tex, NULL, &bullets[i]->vec);
	}
}

//Player
void initPlayer()
{
	player.tex = player_tex;
	player.speed = 250;
	player.pos.x = 640 / 2 - player_surface->w / 2;
	player.pos.y = (480 - 60) - player_surface->h / 2;
	player.size = { 0, 0, player_surface->w, player_surface->h };
	player.vec = { player.pos.x, player.pos.y, player.size.w, player.size.h };
	player.alive = 1;
}

void input()
{
	const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);

	//continuous-response keys
	if (currentKeyStates[SDL_SCANCODE_RIGHT] || move_right)
	{
		player.pos.x += player.speed * deltaTime;
	}
	else if (currentKeyStates[SDL_SCANCODE_LEFT] || move_left)
	{
		player.pos.x -= player.speed * deltaTime;
	}
}

void fire()
{
	addBullet(player.pos.x + player.size.w / 2 - 3, player.pos.y);
	Mix_PlayChannel(1, snd_blaster, 0);
}

void updatePlayer()
{
	input();

	if (player.pos.x <= 0)
	{
		player.pos.x = 0;
	}
	else if (player.pos.x >= 640 - player.size.w)
	{
		player.pos.x = 640 - player.size.w;
	}

	player.hitbox = { 0, 0, player.pos.x, player.pos.y };
	player.vec = { player.pos.x, player.pos.y, player.size.w, player.size.h };
}

//Explosion
void addExplo(float x, float y)
{
	int found = -1;
	for (int i = 0; i < MAX_EXPLO; i++)
	{
		if (explo[i] == NULL)
		{
			found = i;
			break;
		}
	}

	if (found >= 0)
	{
		int i = found;
		explo[i] = (explo_t*)malloc(sizeof(explo_t));
		explo[i]->tex = explo_tex;
		explo[i]->pos = { (int)x, (int)y };
		explo[i]->size = { 0, 0, explo_surface->w, explo_surface->h };
		explo[i]->rect = { 0, 0, 128, 128 };
		explo[i]->animationFinished = 0;
		explo[i]->currentFrame_ex = 0.0f;
	}
}

void animatorExplo()
{
	for (int e = 0; e < MAX_EXPLO; e++) if (explo[e])
	{
		explo[e]->currentFrame_ex += 30.0f * deltaTime;
		explo[e]->rect.x = (int)explo[e]->currentFrame_ex * 128.0f;

		if ((int)explo[e]->currentFrame_ex >= 16)
		{
			explo[e]->animationFinished = 1;
		}
	}
}

void updateExplo()
{
	animatorExplo();

	for (int i = 0; i < MAX_EXPLO; i++) if (explo[i])
	{
		explo[i]->vec = { explo[i]->pos.x, explo[i]->pos.y, 0, 0 };
		explo[i]->spritecut = { explo[i]->pos.x, explo[i]->pos.y, explo[i]->rect.w, explo[i]->rect.h };

		if (explo[i]->animationFinished == 1)
		{
			free(explo[i]);
			explo[i] = NULL;
		}
	}
}

void drawExplo()
{
	for (int i = 0; i < MAX_EXPLO; i++) if (explo[i])
	{
		SDL_RenderCopy(renderer, explo[i]->tex, &explo[i]->rect, &explo[i]->spritecut);
	}
}

void removeExplo(int i)
{
	if (explo[i])
	{
		free(explo[i]);
		explo[i] = NULL;
	}
}

//something something
void updateLogic()
{
	for (int i = 0; i < MAX_BULLETS; i++) if (bullets[i])
	{
		for (int e = 0; e < MAX_ENEMIES; e++) if (enemy[e])
		{
			if (SDL_HasIntersection(&bullets[i]->vec, &enemy[e]->spritecut) && enemy[e]->alive == 1)
			{
				enemy[e]->alive = 0;
				addExplo(bullets[i]->pos.x - 128 / 2, bullets[i]->pos.y - 128 / 2);
				removeBullet(i);
				Mix_PlayChannel(0, snd_explo, 0);
				score += 100;
				enemies_killed--;
				break;
			}
		}
	}

	for(int e = 0; e < MAX_ENEMIES; e++) if (enemy[e])
	{
		if(enemy[e]->alive == 0)
		{
			removeEnemy(e);
		}
	}

	for (int b = 0; b < MAX_ENEMY_BULLETS; b++) if (enemy_bullets[b])
	{
		if (SDL_HasIntersection(&enemy_bullets[b]->vec, &player.vec) && player.alive == 1)
		{
			player.alive = 0;
			addExplo(player.pos.x - 128 / 2, player.pos.y - 128 / 2);
			removeEnemyBullet(b);
			Mix_PlayChannel(0, snd_explo, 0);
			SDL_HapticRumblePlay(haptic, 0.5, 500); // Play effect at 50% strength for 2 seconds
			break;
		}
	}
}

void reset()
{
	enemies_killed = MAX_ENEMIES;
	gameover = 0;
	score = 0;
	memset(enemy, 0, sizeof(enemy));
	currentFrame = 0.0f;
	memset(bullets, 0, sizeof(bullets));
	memset(enemy_bullets, 0, sizeof(enemy_bullets));
	memset(explo, 0, sizeof(explo));
	rowCount = 0;
	itemCount = 0;
	initEnemies();
	player.pos.x = 640 / 2 - player.size.w / 2;
	player.alive = 1;
}

int main(int argc, char* argv[])
{
	//init SDL and setup window
	SDL_Init(SDL_INIT_EVERYTHING);
	window = SDL_CreateWindow("SDL2 Space Invaders", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		640, 480, SDL_WINDOW_SHOWN); //SDL_WINDOW_FULLSCREEN
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	SDL_RenderSetLogicalSize(renderer, 640, 480); //original size upscaled to window resolution

	//Initialize SDL_mixer
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) == -1)
	{
		return 1;
	}

	//Set up TTf stuff
	if (TTF_Init() == -1)
	{
		return 1;
	}

	//Get the first available controller
	for (int i = 0; i < SDL_NumJoysticks(); ++i)
	{
		if (SDL_IsGameController(i))
		{
			controller = SDL_GameControllerOpen(i);
			if (controller)
			{
				const char* name = SDL_GameControllerNameForIndex(i);
				if (name)
					printf("Controller %i has game controller name '%s'\n", i, name);

				// Open the device
				haptic = SDL_HapticOpen(0);
				if (haptic == NULL)
					return -1;

				// Initialize simple rumble
				if (SDL_HapticRumbleInit(haptic) != 0)
					return -1;
				break;
			}
			else
			{
				fprintf(stderr, "Could not open Controller %i: %s\n", i, SDL_GetError());
			}
		}
	}

	//prepare assets
	load_assets();

	//load audio
	snd_blaster = Mix_LoadWAV("rd/blaster.mp3");
	snd_explo = Mix_LoadWAV("rd/explode1.wav");
	snd_pusher = Mix_LoadWAV("rd/pusher.wav");
	music = Mix_LoadMUS("rd/bodenstaendig.mp3");

	//Play the music
	if (Mix_PlayMusic(music, -1) == -1)
	{
		return 1;
	}

	//splash screen
	SDL_RenderCopy(renderer, fmg_splash_tex, NULL, NULL);
	SDL_RenderPresent(renderer);
	SDL_Delay(2000);

	//init stuff
	initEnemies();
	initPlayer();

	//init font stuff
	vermin_ttf = TTF_OpenFont("rd/vermin_vibes_1989.ttf", 24);
	char textBuffer[64];
	sprintf(textBuffer, "SCORE: % 05d", score);
	scoreText = TTF_RenderText_Solid(vermin_ttf, textBuffer, { 255, 255, 255 });
	font_tex = SDL_CreateTextureFromSurface(renderer, scoreText);
	score_pos = { 640 - scoreText->w - 20, 20 };
	score_tex_pos = { score_pos.x, score_pos.y, scoreText->w, scoreText->h };
	SDL_FreeSurface(scoreText);

	while (quit == 0)
	{
		deltaTime = ((float)SDL_GetTicks() / 1000.0f) - lastTick;

		//Handle events on queue
		while (SDL_PollEvent(&e) != 0)
		{
			switch (e.type)
			{
			case SDL_KEYDOWN:
				if (e.key.keysym.sym == SDLK_SPACE && player.alive == 1)
				{
					fire();
				}

				if (e.key.keysym.sym == SDLK_ESCAPE)
				{
					quit = 1;
				}

				if (e.key.keysym.sym == SDLK_RETURN && gameover == 1)
				{
					reset();
				}

				break;

			case SDL_CONTROLLERBUTTONDOWN:
				if (e.type == SDL_CONTROLLERBUTTONDOWN && e.cbutton.button == SDL_CONTROLLER_BUTTON_A && player.alive == 1)
				{
					fire();
				}

				if (e.type == SDL_CONTROLLERBUTTONDOWN && e.cbutton.button == SDL_CONTROLLER_BUTTON_BACK)
				{
					quit = 1;
				}

				if (e.type == SDL_CONTROLLERBUTTONDOWN && e.cbutton.button == SDL_CONTROLLER_BUTTON_START && gameover == 1)
				{
					reset();
				}

				if (e.type == SDL_CONTROLLERBUTTONDOWN && e.cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_RIGHT)
				{
					move_right = 1;
				}
				else if (e.type == SDL_CONTROLLERBUTTONDOWN && e.cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_LEFT)
				{
					move_left = 1;
				}

				break;

			case SDL_CONTROLLERBUTTONUP:
				//continuous-response keys
				if (e.type == SDL_CONTROLLERBUTTONUP && e.cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_RIGHT)
				{
					move_right = 0;
				}
				else if (e.type == SDL_CONTROLLERBUTTONUP && e.cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_LEFT)
				{
					move_left = 0;
				}

				break;
			case SDL_CONTROLLERAXISMOTION:
				// handle axis motion
				if (e.caxis.axis == SDL_CONTROLLER_AXIS_LEFTX)
				{
					LX = (float)e.caxis.value / 32768.0f;
					//printf("LX: %f\n", (float)LX);
				}

				if (e.caxis.axis == SDL_CONTROLLER_AXIS_LEFTX && LX <= -0.5f)
				{
					move_right = 0;
					move_left = 1;
				}

				if (e.caxis.axis == SDL_CONTROLLER_AXIS_LEFTX && LX >= 0.5f)
				{
					move_left = 0;
					move_right = 1;
				}

				if (e.caxis.axis == SDL_CONTROLLER_AXIS_LEFTX && LX < 0.5f && e.caxis.axis == SDL_CONTROLLER_AXIS_LEFTX
					&& LX > -0.5f)
				{
					move_left = 0;
					move_right = 0;
				}
				break;
			}
		}

		//draw stuff
		SDL_RenderClear(renderer);

		updateExplo();
		updateBullet();
		updateEnemyBullet();
		updateEnemies();

		if (player.alive == 1)
		{
			updatePlayer(); //player
		}

		updateLogic();

		for (int e = 0; e < MAX_ENEMIES; e++) if (enemy[e])
		{
			enemy[e]->rect.x = (int)currentFrame * 32;
		}

		SDL_RenderCopy(renderer, space3_tex, NULL, NULL);

		drawExplo();
		drawEnemies();
		drawBullet();
		drawEnemyBullet();

		if (player.alive == 1)
		{
			SDL_RenderCopy(renderer, player.tex, NULL, &player.vec);
		}
		else
		{
			SDL_RenderCopy(renderer, gameover_tex, NULL, NULL);
			gameover = 1;
		}

		if (enemies_killed <= 0)
		{
			SDL_RenderCopy(renderer, win_tex, NULL, NULL);
			gameover = 1;
		}

		//this ugly block is updating the score
		sprintf(textBuffer, "SCORE: % 05d", score);
		SDL_RenderCopy(renderer, font_tex, NULL, &score_tex_pos);
		scoreText = NULL;
		scoreText = TTF_RenderText_Solid(vermin_ttf, textBuffer, { 255, 255, 255 });
		font_tex = SDL_CreateTextureFromSurface(renderer, scoreText);
		score_tex_pos = { score_pos.x, score_pos.y, scoreText->w, scoreText->h };
		SDL_FreeSurface(scoreText);

		//Update Screen
		SDL_RenderPresent(renderer);

		lastTick = (float)SDL_GetTicks() / 1000.0f;
		SDL_Delay(30);
	}

	SDL_GameControllerClose(controller);
	SDL_DestroyTexture(player_tex);
	SDL_DestroyTexture(bullet_tex);
	SDL_DestroyTexture(enemy_bullet_tex);
	SDL_DestroyTexture(explo_tex);
	SDL_DestroyTexture(fmg_splash_tex);
	SDL_DestroyTexture(gameover_tex);
	SDL_DestroyTexture(win_tex);
	SDL_DestroyTexture(font_tex);
	TTF_CloseFont(vermin_ttf);
	Mix_FreeMusic(music);
	Mix_FreeChunk(snd_blaster);
	Mix_FreeChunk(snd_explo);
	Mix_FreeChunk(snd_pusher);
	memset(enemy, 0, sizeof(enemy));
	memset(bullets, 0, sizeof(bullets));
	memset(enemy_bullets, 0, sizeof(enemy_bullets));
	memset(explo, 0, sizeof(explo));
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	//Quit SDL
	SDL_Quit();

	return 0;
}