// sdl-space-invaders.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_image.h>

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
SDL_Surface* space = NULL;
SDL_Surface* screen = NULL;
SDL_Surface* gWindow = NULL;
SDL_Joystick* joystick;

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
int DeltaTime;
int lastTick;
int score = 0;

//Music
Mix_Music* music = NULL;
Mix_Chunk* snd_pusher = NULL;
Mix_Chunk* snd_blaster = NULL;
Mix_Chunk* snd_explo = NULL;

bool move_left = 0;
bool move_right = 0;

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
		enemy_bullets[i]->tex = IMG_Load("rd/enemy-bullet.png");
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
	int i;
	for (i = 0; i < MAX_ENEMY_BULLETS; i++) if (enemy_bullets[i])
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
	int i;
	for (i = 0; i < MAX_ENEMY_BULLETS; i++) if (enemy_bullets[i])
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
		enemy[i]->tex = IMG_Load("rd/invader32x32x4.png"); //invader32x32x4
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
	int i;
	for (i = 0; i < MAX_ENEMIES; i++)
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

	const int moveSpeed = 1;

	int e;
	for (e = 0; e < MAX_ENEMIES; e++) if (enemy[e]) {
		if (enemy[e]->goLeft == 0)
		{
			enemy[e]->pos.x += moveSpeed;
		}

		if (enemy[e]->goLeft == 1)
		{
			enemy[e]->pos.x -= moveSpeed;
		}

		if (enemy[e]->pos.x >= 640 - (enemy[e]->rect.w + enemy[e]->rowPosID) && enemy[e]->goLeft == 0)
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
	int e;
	for (e = 0; e < MAX_ENEMIES; e++) if (enemy[e]) {
		if (enemy[e]->alive == 1) {
			//SDL_FillRect(screen, &enemy[e]->hitbox, SDL_MapRGB(screen->format, 255, 0, 0));
			SDL_BlitSurface(enemy[e]->tex, &enemy[e]->rect, screen, &enemy[e]->pos);
		}
	}
}

/*
   Player
*/

void initPlayer()
{
	player.tex = IMG_Load("rd/player.png");
	p_move = 640 / 2 - player.tex->w / 2;
	player.pos.y = (480 - 60) - player.tex->h / 2;
	player.hitbox.w = player.tex->w;
	player.hitbox.h = player.tex->h;
	player.hitbox.x = player.pos.x;
	player.hitbox.y = player.pos.y;
	player.alive = 1;
}

void input()
{
	if (move_left)
		p_move -= 15;

	if (move_right)
		p_move += 15;
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
	else if (player.pos.x >= 640 - player.tex->w)
	{
		player.pos.x = 640 - player.tex->w;
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
		bullets[i]->tex = IMG_Load("rd/bullet.png");
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
	int i;
	for (i = 0; i < MAX_BULLETS; i++) if (bullets[i])
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
	int i;
	for (i = 0; i < MAX_BULLETS; i++) if (bullets[i])
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
		explo[i]->tex = IMG_Load("rd/explode.png");
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
	int e;
	for (e = 0; e < MAX_EXPLO; e++) if (explo[e]) {
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

	int i;
	for (i = 0; i < MAX_EXPLO; i++) if (explo[i])
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
	int i;
	for (i = 0; i < MAX_EXPLO; i++) if (explo[i])
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
	int i, e;
	for (i = 0; i < MAX_BULLETS; i++) if (bullets[i])
	{
		for (e = 0; e < MAX_ENEMIES; e++) if (enemy[e]) {
			if (bullets[i]->pos.x > enemy[e]->pos.x && bullets[i]->pos.x < enemy[e]->pos.x + enemy[e]->hitbox.w &&
				bullets[i]->pos.y > enemy[e]->pos.y && bullets[i]->pos.y < enemy[e]->pos.y + enemy[e]->hitbox.h &&
				enemy[e]->alive == 1)
			{
				enemy[e]->alive = 0;
				addExplo(bullets[i]->pos.x - 128 / 2, bullets[i]->pos.y - 128 / 2);
				removeBullet(i);
				Mix_PlayChannel(0, snd_explo, 0);
				score += 100;
				//printf("BOOM!\n");
				break;
			}
		}
	}


	int b;
	for (b = 0; b < MAX_ENEMY_BULLETS; b++) if (enemy_bullets[b])
	{
		if (enemy_bullets[b]->pos.x > player.pos.x && enemy_bullets[b]->pos.x < player.pos.x + player.hitbox.w &&
			enemy_bullets[b]->pos.y > player.pos.y && enemy_bullets[b]->pos.y < player.pos.y + player.hitbox.h &&
			player.alive == 1)
		{
			player.alive = 0;
			addExplo(player.pos.x - 128 / 2, player.pos.y - 128 / 2);
			removeEnemyBullet(i);
			Mix_PlayChannel(0, snd_explo, 0);
			//printf("BOOM!\n");
			break;
		}
	}

}

int main(int argc, char* argv[]) {

	//Start SDL
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK);
	SDL_ShowCursor(SDL_DISABLE);

	Uint32          colorkey;
	SDL_Surface * icon;
	icon = IMG_Load("rd/icon.png");
	colorkey = SDL_MapRGB(icon->format, 255, 0, 255);

	//Set up screen
	Uint32 flags = SDL_SWSURFACE | SDL_DOUBLEBUF;// | SDL_FULLSCREEN;// | SDL_INIT_JOYSTICK; // | SDL_FULLSCREEN;
	screen = SDL_SetVideoMode(640, 480, 16, flags);

	//printf("%i joysticks were found.\n\n", SDL_NumJoysticks());
	printf("The names of the joysticks are:\n");
	for (int i = 0; i < SDL_NumJoysticks(); i++)
	{
		printf("    %s\n", SDL_JoystickName(i));
	}

	SDL_Joystick* joystick;

	SDL_JoystickEventState(SDL_ENABLE);
	joystick = SDL_JoystickOpen(0);

	TTF_Init();

	//Initialize SDL_mixer
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) == -1)
	{
		return false;
	}

	music = Mix_LoadMUS("rd/bodenstaendig.ogg");

	SDL_Surface* fmg_splash = IMG_Load("rd/fmg_splash.png");
	SDL_BlitSurface(fmg_splash, NULL, screen, NULL);
	SDL_Flip(screen);
	SDL_Delay(2000);
	SDL_FreeSurface(fmg_splash);

	//Load image
	space = IMG_Load("rd/space3.png");

	//init stuff
	initEnemies();
	initPlayer();

	//
	TTF_Font* vermin_ttf = TTF_OpenFont("rd/vermin_vibes_1989.ttf", 24);
	SDL_Rect score_pos;
	SDL_Color Color = { 255, 255, 255 };
	char textBuffer[64];
	sprintf(textBuffer, "SCORE: % 05d", score);
	SDL_Surface* scoreText = TTF_RenderText_Solid(vermin_ttf, textBuffer, Color);
	score_pos.x = 640 - scoreText->w;

	SDL_Surface* youWin = TTF_RenderText_Solid(vermin_ttf, "You Win!", Color);
	SDL_Rect youWin_pos;
	youWin_pos.x = 640 / 2 - youWin->w / 2;
	youWin_pos.y = 480 / 2 - youWin->h / 2;

	SDL_Surface* gameOver = TTF_RenderText_Solid(vermin_ttf, "Game Over!", Color);
	SDL_Rect game_over_pos;
	game_over_pos.x = 640 / 2 - gameOver->w / 2;
	game_over_pos.y = 480 / 2 - gameOver->h / 2;

	//load audio
	snd_blaster = Mix_LoadWAV("rd/blaster.ogg");
	snd_explo = Mix_LoadWAV("rd/explode1.ogg");
	snd_pusher = Mix_LoadWAV("rd/pusher.ogg");

	//Play the music
	if (Mix_PlayMusic(music, -1) == -1)
	{
		return 1;
	}

	while (1)
	{
		DeltaTime = (SDL_GetTicks() / 1000) - lastTick;

		//Handle events on queue
		while (SDL_PollEvent(&e) != 0)
		{
			switch (e.type)
			{
			case SDL_KEYDOWN:
				if (e.key.keysym.sym == SDLK_RIGHT)
				{
					move_right = 1;
				}
				else if (e.key.keysym.sym == SDLK_LEFT)
				{
					move_left = 1;
				}
				else if (e.key.keysym.sym == SDLK_SPACE && player.alive == 1)
				{
					addBullet(player.pos.x + player.tex->w / 2 - 3, player.pos.y);
					Mix_PlayChannel(-1, snd_blaster, 0);
				}
				break;
			case SDL_KEYUP:
				if (e.key.keysym.sym == SDLK_RIGHT)
				{
					move_right = 0;
				}
				else if (e.key.keysym.sym == SDLK_LEFT)
				{
					move_left = 0;
				}
				break;
			case SDL_JOYBUTTONDOWN:
				if (e.jbutton.button == 0 && player.alive == 1)
				{
					addBullet(player.pos.x + player.tex->w / 2 - 3, player.pos.y);		
					Mix_PlayChannel(-1, snd_blaster, 0);
				}
				break;
			case SDL_JOYHATMOTION:
				if (e.jhat.value == SDL_HAT_LEFT)
				{
					move_left = 1;
				}
				else
				{
					move_left = 0;
				}
				 
				if (e.jhat.value == SDL_HAT_RIGHT)
				{
					move_right = 1;
				}
				else
				{
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

		int e;
		for (e = 0; e < MAX_ENEMIES; e++) if (enemy[e]) {
			enemy[e]->rect.x = currentFrame * 32;
		}

		SDL_BlitSurface(space, NULL, screen, NULL);
		drawExplo();
		drawEnemies();
		drawBullet();
		drawEnemyBullet();


		if (player.alive == 1) {
			//SDL_FillRect(screen, &player.hitbox, SDL_MapRGB(screen->format, 255, 0, 0));
			SDL_BlitSurface(player.tex, NULL, screen, &player.pos);
		}
		else
		{
			SDL_BlitSurface(gameOver, NULL, screen, &game_over_pos);
		}

		if (score >= 4000)
			SDL_BlitSurface(youWin, NULL, screen, &youWin_pos);

		//this ugly block is updating the score
		sprintf(textBuffer, "SCORE: % 05d", score);
		SDL_BlitSurface(scoreText, NULL, screen, &score_pos);
		scoreText = NULL;
		scoreText = TTF_RenderText_Solid(vermin_ttf, textBuffer, Color);

		//Update Screen
		SDL_Flip(screen);

		lastTick = (SDL_GetTicks() / 1000);
		SDL_Delay(30);
	}

	SDL_FreeSurface(player.tex);
	SDL_FreeSurface(space);

	//Pause
	//SDL_Delay(2000);

	//Free the loaded image
	//SDL_FreeSurface( hello );

	//Quit SDL
	//SDL_Quit();

	return 0;
}
