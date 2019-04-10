#include <cstdlib>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <switch.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

struct player_t {
	SDL_Rect hitbox;
	SDL_Rect pos;
	SDL_Surface* tex;
	int alive; //bool
};

struct bullet_t
{
	SDL_Rect hitbox;
	SDL_Rect pos;
	SDL_Surface* tex;
};

struct enemy_bullet_t
{
	SDL_Rect hitbox;
	SDL_Rect pos;
	SDL_Surface* tex;
};

struct explo_t
{
	SDL_Rect pos;
	SDL_Rect rect;
	SDL_Surface* tex;
	int animationFinished; //bool
	int currentFrame_ex;
};

struct enemy_t {
	SDL_Rect hitbox;
	SDL_Rect pos;
	SDL_Rect rect;
	SDL_Surface* tex;
	int goLeft; //bool
	int startPos;
	int rowPosID;
	int alive; //bool
	float shootTimer;
	int shoot; //bool
	float shootTimeLimit;
};

//base SDL stuff
SDL_Event e;
SDL_Surface* screen = NULL;
SDL_Window* gWindow = NULL;

SDL_GameController* controller = NULL;
#define GAMEPAD_DEADZONE  32768.0f
float LX = 0;

//player
#define MAX_BULLETS 50
struct player_t player;
int p_move;

//enemy
#define MAX_ENEMY_BULLETS 100
#define MAX_ENEMIES 40
struct enemy_t* enemy[MAX_ENEMIES] = { NULL };
int currentFrame = 0;

//stuff
struct bullet_t* bullets[MAX_BULLETS] = { NULL };
struct enemy_bullet_t* enemy_bullets[MAX_ENEMY_BULLETS] = { NULL };
#define MAX_EXPLO 100
struct explo_t* explo[MAX_EXPLO] = { NULL };
int rowCount = 0;
int itemCount = 0;
float DeltaTime;
float lastTick;
int score = 0;
int gameover = 0;
int quit = 0;
int dead_enemies = 0;
bool move_left = 0;
bool move_right = 0;

//Music
Mix_Music* music = NULL;
Mix_Chunk* snd_pusher = NULL;
Mix_Chunk* snd_blaster = NULL;
Mix_Chunk* snd_explo = NULL;

//sprites and stuff
SDL_Surface* space3_tex2d;
SDL_Surface* enemy_tex2d;
SDL_Surface* player_tex2d;
SDL_Surface* bullet_tex2d;
SDL_Surface* enemy_bullet_tex2d;
SDL_Surface* explo_tex2d;
SDL_Surface* fmg_splash_tex2d;
SDL_Surface* gameover_tex2D;
SDL_Surface* win_tex2d;
//SDL_Surface* scoreFont;

void load_assets()
{
	space3_tex2d = IMG_Load("/rd/space3.png");
	enemy_tex2d = IMG_Load("/rd/invader32x32x4.png"); //invader32x32x4
	player_tex2d = IMG_Load("/rd/player.png");
	bullet_tex2d = IMG_Load("/rd/bullet.png");
	enemy_bullet_tex2d = IMG_Load("/rd/enemy-bullet.png");
	explo_tex2d = IMG_Load("/rd/explode.png");
	fmg_splash_tex2d = IMG_Load("/rd/fmg_splash.png");
	gameover_tex2D = IMG_Load("/rd/gameover_ui.png");
	win_tex2d = IMG_Load("/rd/win_ui.png");
	//scoreFont = IMG_Load();
}

/*
Enemy Bullets
*/

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
		//printf("Pew Pew!");
		int i = found;
		enemy_bullets[i] = (enemy_bullet_t*)malloc(sizeof(enemy_bullet_t));
		enemy_bullets[i]->tex = enemy_bullet_tex2d;
		enemy_bullets[i]->pos.x = x;
		enemy_bullets[i]->pos.y = y;
		enemy_bullets[i]->hitbox.w = enemy_bullets[i]->tex->w;
		enemy_bullets[i]->hitbox.h = enemy_bullets[i]->tex->h;
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
		enemy_bullets[i]->pos.y += 15;

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
		SDL_BlitSurface(enemy_bullets[i]->tex, NULL, screen, &enemy_bullets[i]->pos);
	}
}

/*
  Enemies
*/

void addEnemy()
{
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
		//printf("GABAAA!!");
		int i = found;
		enemy[i] = (enemy_t*)malloc(sizeof(enemy)); //visual studio needs that "stupid" cast to operate >_>
		enemy[i]->tex = enemy_tex2d;
		enemy[i]->alive = 1;
		enemy[i]->rect.h = 32;
		enemy[i]->rect.w = 32;
		enemy[i]->rect.x = 0;
		enemy[i]->rect.y = 0;
		enemy[i]->pos.x = itemCount * 40;
		enemy[i]->pos.y = 40 * rowCount;
		enemy[i]->startPos = enemy[i]->pos.x;
		enemy[i]->rowPosID = 40 * (11 - itemCount);
		enemy[i]->goLeft = 0;
		enemy[i]->hitbox.w = enemy[i]->rect.w;
		enemy[i]->hitbox.h = enemy[i]->rect.h;
		enemy[i]->hitbox.x = enemy[i]->pos.x;
		enemy[i]->hitbox.y = enemy[i]->pos.y;
		enemy[i]->shoot = 0;
		enemy[i]->shootTimer = 0;
		enemy[i]->shootTimeLimit = (rand() % (20 - 3)) + 3; // MAX - MIN + MIN
	}
}

void initEnemies()
{
	for (int i = 0; i < MAX_ENEMIES; i++)
	{

		if (i % 10 == 0)
		{
			//printf("new row");
			itemCount = 0;
			rowCount++;
		}

		itemCount++;
		addEnemy();
	}
}

void animatorEnemies()
{
	currentFrame += 1;

	if (currentFrame >= 4)
	{
		currentFrame = 0;
	}

	int e;
	for (e = 0; e < MAX_ENEMIES; e++) if (enemy[e]) {
		enemy[e]->rect.x = currentFrame * 32;
	}
}

void updateEnemies()
{
	animatorEnemies();

	const int moveSpeed = 3;

	for (int e = 0; e < MAX_ENEMIES; e++) if (enemy[e]) {
		if (enemy[e]->goLeft == 0)
		{
			enemy[e]->pos.x += moveSpeed;
		}

		if (enemy[e]->goLeft == 1)
		{
			enemy[e]->pos.x -= moveSpeed;
		}

		if (enemy[e]->pos.x >= 850 - (enemy[e]->rect.w + enemy[e]->rowPosID) && enemy[e]->goLeft == 0)
		{
			enemy[e]->goLeft = 1;
		}

		if (enemy[e]->pos.x <= enemy[e]->startPos + enemy[e]->rect.w && enemy[e]->goLeft == 1)
		{
			enemy[e]->goLeft = 0;
		}

		enemy[e]->shootTimer += 1 * DeltaTime;

		//c = enemy[1]->shootTimer += 1 * DeltaTime;;
		//printf("%0.8f\n", c);

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
			Mix_PlayChannel(-1, snd_pusher, 0);
		}
		enemy[e]->hitbox.x = enemy[e]->pos.x;
		enemy[e]->hitbox.y = enemy[e]->pos.y;
	}
}

void drawEnemies()
{
	for (int e = 0; e < MAX_ENEMIES; e++) if (enemy[e]) {
		if (enemy[e]->alive == 1) {
			//SDL_FillRect(screen, &enemy[e]->hitbox, SDL_MapRGB(screen->format, 255, 0, 0));
			SDL_BlitSurface(enemy[e]->tex, &enemy[e]->rect, screen, &enemy[e]->pos);
		}
	}
}

/*void removeEnemy(int i)
{
	if (enemy[i])
	{
		free(enemy[i]);
		enemy[i] = NULL;
		dead_enemies++;
	}
}*/

/*
   Player
*/

void initPlayer()
{
	player.tex = player_tex2d;
	p_move = 850 / 2 - player.tex->w / 2;
	player.pos.y = (480 - 60) - player.tex->h / 2;
	player.hitbox.w = player.tex->w;
	player.hitbox.h = player.tex->h;
	player.hitbox.x = player.pos.x;
	player.hitbox.y = player.pos.y;
	player.alive = 1;
}

void input()
{
	const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);

	//continuous-response keys
	if (currentKeyStates[SDL_SCANCODE_RIGHT] || move_right)
	{
		p_move += 15;
	}
	else if (currentKeyStates[SDL_SCANCODE_LEFT] || move_left)
	{
		p_move -= 15;
	}
}

void updatePlayer()
{
	input();

	player.pos.x = p_move;

	player.hitbox.x = player.pos.x;
	player.hitbox.y = player.pos.y;

	if (player.pos.x <= 0)
	{
		player.pos.x = 0;
	}
	else if (player.pos.x >= 850 - player.tex->w)
	{
		player.pos.x = 850 - player.tex->w;
	}
}

/*
   Bullet
*/

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
		bullets[i]->tex = bullet_tex2d;
		bullets[i]->pos.x = x;
		bullets[i]->pos.y = y;
		bullets[i]->hitbox.w = 6;
		bullets[i]->hitbox.h = 36;
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
		bullets[i]->pos.y -= 15;

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
		SDL_BlitSurface(bullets[i]->tex, NULL, screen, &bullets[i]->pos);
	}
}

/*
Explosion
*/

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
		explo[i]->tex = explo_tex2d;
		explo[i]->pos.x = x;
		explo[i]->pos.y = y;
		explo[i]->rect.w = 128;
		explo[i]->rect.h = 129;
		explo[i]->rect.x = 0;
		explo[i]->rect.y = 0;
		explo[i]->animationFinished = 0;
		explo[i]->currentFrame_ex = 0;
	}
}

void animatorExplo()
{
	for (int e = 0; e < MAX_EXPLO; e++) if (explo[e]) {
		explo[e]->currentFrame_ex += 1;
		explo[e]->rect.x = explo[e]->currentFrame_ex * 128;

		if (explo[e]->currentFrame_ex >= 16)
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
		SDL_BlitSurface(explo[i]->tex, &explo[i]->rect, screen, &explo[i]->pos);
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

/*
  Etc
*/

void updateLogic()
{
	for (int i = 0; i < MAX_BULLETS; i++) if (bullets[i])
	{
		for (int e = 0; e < MAX_ENEMIES; e++) if (enemy[e]) {
			if (bullets[i]->pos.x > enemy[e]->pos.x && bullets[i]->pos.x < enemy[e]->pos.x + enemy[e]->hitbox.w &&
				bullets[i]->pos.y > enemy[e]->pos.y && bullets[i]->pos.y < enemy[e]->pos.y + enemy[e]->hitbox.h &&
				enemy[e]->alive == 1)
			{
				enemy[e]->alive = 0;
				addExplo(bullets[i]->pos.x - 128 / 2, bullets[i]->pos.y - 128 / 2);
				removeBullet(i);
				Mix_PlayChannel(0, snd_explo, 0);
				score += 100;
				dead_enemies++;
				//printf("BOOM!\n");
				break;
			}
		}
	}

	for (int b = 0; b < MAX_ENEMY_BULLETS; b++) if (enemy_bullets[b])
	{
		if (enemy_bullets[b]->pos.x > player.pos.x && enemy_bullets[b]->pos.x < player.pos.x + player.hitbox.w &&
			enemy_bullets[b]->pos.y > player.pos.y && enemy_bullets[b]->pos.y < player.pos.y + player.hitbox.h &&
			player.alive == 1)
		{
			player.alive = 0;
			addExplo(player.pos.x - 128 / 2, player.pos.y - 128 / 2);
			removeEnemyBullet(b);
			Mix_PlayChannel(0, snd_explo, 0);
			//printf("BOOM!\n");
			break;
		}
	}
}

void reset()
{
	dead_enemies = 0;
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
	p_move = 850 / 2 - player.tex->w / 2;
	player.alive = 1;
}

int main(int argc, char *argv[])
{
	//Start SDL
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER);
	SDL_ShowCursor(SDL_DISABLE);

	//Set up screen
	gWindow = SDL_CreateWindow("Space Invaders", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_SHOWN);
	screen = SDL_GetWindowSurface(gWindow);

	//Set up TTf stuff
	TTF_Init();

	//Get the first available controller
	for (int i = 0; i < SDL_NumJoysticks(); ++i) {
		if (SDL_IsGameController(i)) {
			controller = SDL_GameControllerOpen(i);
			if (controller) {
				const char* name = SDL_GameControllerNameForIndex(i);
				if (name)
					printf("Controller %i has game controller name '%s'\n", i, name);
				break;
			}
			else {
				fprintf(stderr, "Could not open Controller %i: %s\n", i, SDL_GetError());
			}
		}
	}

	//Initialize SDL_mixer
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) == -1)
	{
		return false;
	}

	load_assets();

	music = Mix_LoadMUS("/rd/bodenstaendig.mp3");

	SDL_BlitSurface(fmg_splash_tex2d, NULL, screen, NULL);
	SDL_UpdateWindowSurface(gWindow);
	SDL_Delay(3000);
	SDL_FreeSurface(fmg_splash_tex2d);

	//init stuff
	initEnemies();
	initPlayer();

	TTF_Font* vermin_ttf = TTF_OpenFont("/rd/vermin_vibes_1989.ttf", 24);
	SDL_Rect score_pos;
	SDL_Color Color = { 255, 255, 255 };
	char textBuffer[64];
	sprintf(textBuffer, "SCORE: % 05d", score);
	SDL_Surface* scoreText = TTF_RenderText_Solid(vermin_ttf, textBuffer, Color);
	score_pos.x = 850 - scoreText->w - 10;
	score_pos.y = 10;

	SDL_Surface* youWin = TTF_RenderText_Solid(vermin_ttf, "You Win!", Color);
	SDL_Rect youWin_pos;
	youWin_pos.x = 640 / 2 - youWin->w / 2;
	youWin_pos.y = 480 / 2 - youWin->h / 2;

	SDL_Surface* gameOver = TTF_RenderText_Solid(vermin_ttf, "Game Over!", Color);
	SDL_Rect game_over_pos;
	game_over_pos.x = 640 / 2 - gameOver->w / 2;
	game_over_pos.y = 480 / 2 - gameOver->h / 2;

	//load audio
	snd_blaster = Mix_LoadWAV("/rd/blaster.mp3");
	snd_explo = Mix_LoadWAV("/rd/explode1.wav");
	snd_pusher = Mix_LoadWAV("/rd/pusher.wav");

	//Play the music
	if (Mix_PlayMusic(music, -1) == -1)
	{
		return 1;
	}

	while (quit == 0)
	{
		DeltaTime = (armGetSystemTick() /  100000000.0) - lastTick;

		//Handle events on queue
		while (SDL_PollEvent(&e) != 0)
		{
			switch (e.type)
			{
			case SDL_KEYDOWN:
				if (e.key.keysym.sym == SDLK_SPACE && player.alive == 1)
				{
					addBullet(player.pos.x + player.tex->w / 2 - 3, player.pos.y);
					Mix_PlayChannel(-1, snd_blaster, 0);
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
					addBullet(player.pos.x + player.tex->w / 2 - 3, player.pos.y);
					Mix_PlayChannel(-1, snd_blaster, 0);
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

			case  SDL_CONTROLLERBUTTONUP:
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
			case SDL_CONTROLLERAXISMOTION:  //https://docs.microsoft.com/en-gb/windows/desktop/xinput/getting-started-with-xinput#dead_zone
				
				// handle axis motion
				if (e.caxis.axis == SDL_CONTROLLER_AXIS_LEFTX)
				{
					LX = (float)e.caxis.value / 32768.0f;
					printf("LX: %f\n", (float)LX);
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
				 
				if (e.caxis.axis == SDL_CONTROLLER_AXIS_LEFTX && LX < 0.5f && e.caxis.axis == SDL_CONTROLLER_AXIS_LEFTX && LX > -0.5f)
				{
					move_left = 0;
					move_right = 0;
				}
				break;
			}
		}

		updateExplo();
		updateBullet();
		updateEnemyBullet();
		updateEnemies();

		if (player.alive == 1)
			updatePlayer(); //player

		updateLogic();

		for (int e = 0; e < MAX_ENEMIES; e++) if (enemy[e]) {
			enemy[e]->rect.x = currentFrame * 32;
		}

		SDL_BlitSurface(space3_tex2d, NULL, screen, NULL);

		drawExplo();
		drawEnemies();
		drawBullet();
		drawEnemyBullet();

		if (player.alive == 1) {
			SDL_BlitSurface(player.tex, NULL, screen, &player.pos);
		}
		else
		{
			//SDL_BlitSurface(gameOver, NULL, screen, &game_over_pos);
			SDL_BlitSurface(gameover_tex2D, NULL, screen, NULL);
			gameover = 1;
		}

		if (dead_enemies == 40)
		{
			//SDL_BlitSurface(youWin, NULL, screen, &youWin_pos);
			SDL_BlitSurface(win_tex2d, NULL, screen, NULL);
			gameover = 1;
		}

		//printf("MAXCOUNT: %d\n", dead_enemies);

		//this ugly block is updating the score
		sprintf(textBuffer, "SCORE: % 05d", score);
		SDL_BlitSurface(scoreText, NULL, screen, &score_pos);
		scoreText = NULL;
		scoreText = TTF_RenderText_Solid(vermin_ttf, textBuffer, Color);

		//Update Screen
		SDL_UpdateWindowSurface(gWindow);

		lastTick = (armGetSystemTick() /  100000000.0);
		SDL_Delay(30);
	}

	SDL_GameControllerClose(controller);
	SDL_FreeSurface(youWin);
	SDL_FreeSurface(gameOver);
	SDL_FreeSurface(player_tex2d);
	SDL_FreeSurface(bullet_tex2d);
	SDL_FreeSurface(enemy_bullet_tex2d);
	SDL_FreeSurface(explo_tex2d);
	SDL_FreeSurface(fmg_splash_tex2d);
	SDL_FreeSurface(gameover_tex2D);
	SDL_FreeSurface(win_tex2d);
	//SDL_FreeSurface(scoreFont);
	TTF_CloseFont(vermin_ttf);
	Mix_FreeMusic(music);
	Mix_FreeChunk(snd_blaster);
	Mix_FreeChunk(snd_explo);
	Mix_FreeChunk(snd_pusher);
	memset(enemy, 0, sizeof(enemy));
	memset(bullets, 0, sizeof(bullets));
	memset(enemy_bullets, 0, sizeof(enemy_bullets));
	memset(explo, 0, sizeof(explo));

	//Quit SDL
	SDL_Quit();

	return 0;
}