// sdl-space-invaders.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_image.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <3ds.h>

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

SDL_Joystick* joystick;
#define GAMEPAD_DEADZONE 32768.0f
float LX = 0;
float LX_HAT = 0;

//player
#define MAX_BULLETS 50
struct player_t player;
int p_move;

//enemy
#define MAX_ENEMY_BULLETS 100
#define MAX_ENEMIES 15
struct enemy_t* enemy[MAX_ENEMIES] = { NULL };
int currentFrame = 0;

//stuff
struct bullet_t* bullets[MAX_BULLETS] = { NULL };
struct enemy_bullet_t* enemy_bullets[MAX_ENEMY_BULLETS] = { NULL };
#define MAX_EXPLO 100
struct explo_t* explo[MAX_EXPLO] = { NULL };
int rowCount = 0;
int itemCount = 0;
char textBuffer[64];
int DeltaTime;
int lastTick;
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
SDL_Surface*  fontTex;
SDL_Surface*  thumbnail_tex2d;

void load_assets()
{
	space3_tex2d = IMG_Load("romfs:/rd/space3.png");
	enemy_tex2d = IMG_Load("romfs:/rd/invader32x32x4.png"); //invader32x32x4
	player_tex2d = IMG_Load("romfs:/rd/player.png");
	bullet_tex2d = IMG_Load("romfs:/rd/bullet.png");
	enemy_bullet_tex2d = IMG_Load("romfs:/rd/enemy-bullet.png");
	explo_tex2d = IMG_Load("romfs:/rd/explode.png");
	fmg_splash_tex2d = IMG_Load("romfs:/rd/fmg_splash.png");
	gameover_tex2D = IMG_Load("romfs:/rd/gameover_ui.png");
	win_tex2d = IMG_Load("romfs:/rd/win_ui.png");
	fontTex = IMG_Load("romfs:/rd/font.png");
	thumbnail_tex2d = IMG_Load("romfs:/rd/thumbnail.png");
}

//fast and dirty bitmap font
void createFont(SDL_Surface* fontTexture, int fontSize, SDL_Rect pos, const char* text)
{
  SDL_Rect fSize;
  fSize.w = fontSize;
  fSize.h = fontSize;

  SDL_Rect font_rect[256];
  SDL_Rect glyph_pos[256];

  int i;
  for (i = 0; i < strlen(text); i++) {

    //printf("%c\n", text[i]);

    glyph_pos[i].x = pos.x + (fontSize * i);
    glyph_pos[i].y = pos.y;

    font_rect[i].w = 16;
    font_rect[i].h = 16;

    switch (text[i])
    {
    case ' ':
      font_rect[i].x = 0;
      font_rect[i].y = 0;
      break;
    case 'A':
      font_rect[i].x = 16;
      font_rect[i].y = 32;
      break;
    case 'B':
      font_rect[i].x = 32;
      font_rect[i].y = 32;
      break;
    case 'C':
      font_rect[i].x = 48;
      font_rect[i].y = 32;
      break;
    case 'D':
      font_rect[i].x = 64;
      font_rect[i].y = 32;
      break;
    case 'E':
      font_rect[i].x = 80;
      font_rect[i].y = 32;
      break;
    case 'F':
      font_rect[i].x = 96;
      font_rect[i].y = 32;
      break;
    case 'G':
      font_rect[i].x = 112;
      font_rect[i].y = 32;
      break;
    case 'H':
      font_rect[i].x = 128;
      font_rect[i].y = 32;
      break;
    case 'I':
      font_rect[i].x = 144;
      font_rect[i].y = 32;
      break;
    case 'J':
      font_rect[i].x = 160;
      font_rect[i].y = 32;
      break;
    case 'K':
      font_rect[i].x = 176;
      font_rect[i].y = 32;
      break;
    case 'L':
      font_rect[i].x = 192;
      font_rect[i].y = 32;
      break;
    case 'M':
      font_rect[i].x = 208;
      font_rect[i].y = 32;
      break;
    case 'N':
      font_rect[i].x = 224;
      font_rect[i].y = 32;
      break;
    case 'O':
      font_rect[i].x = 240;
      font_rect[i].y = 32;
      break;
    case 'P':
      font_rect[i].x = 0;
      font_rect[i].y = 48;
      break;
    case 'Q':
      font_rect[i].x = 16;
      font_rect[i].y = 48;
      break;
    case 'R':
      font_rect[i].x = 32;
      font_rect[i].y = 48;
      break;
    case 'S':
      font_rect[i].x = 48;
      font_rect[i].y = 48;
      break;
    case 'T':
      font_rect[i].x = 64;
      font_rect[i].y = 48;
      break;
    case 'U':
      font_rect[i].x = 80;
      font_rect[i].y = 48;
      break;
    case 'V':
      font_rect[i].x = 96;
      font_rect[i].y = 48;
      break;
    case 'W':
      font_rect[i].x = 112;
      font_rect[i].y = 48;
      break;
    case 'X':
      font_rect[i].x = 128;
      font_rect[i].y = 48;
      break;
    case 'Y':
      font_rect[i].x = 144;
      font_rect[i].y = 48;
      break;
    case 'Z':
      font_rect[i].x = 160;
      font_rect[i].y = 48;
      break;
    case '(':
      font_rect[i].x = 352;
      font_rect[i].y = 96;
      break;
    case '/':
      font_rect[i].x = 384;
      font_rect[i].y = 96;
      break;
    case ')':
      font_rect[i].x = 416;
      font_rect[i].y = 96;
      break;
    case 'a':

      break;
    case 'b':

      break;
    case 'c':

      break;
    case 'd':

      break;
    case 'e':

      break;
    case 'f':

      break;
    case 'g':

      break;
    case 'h':

      break;
    case 'i':

      break;
    case 'j':

      break;
    case 'k':

      break;
    case 'l':

      break;
    case 'm':

      break;
    case 'n':

      break;
    case 'o':

      break;
    case 'p':

      break;
    case 'q':

      break;
    case 'r':

      break;
    case 's':

      break;
    case 't':

      break;
    case 'u':

      break;
    case 'v':

      break;
    case 'w':

      break;
    case 'x':

      break;
    case 'y':

      break;
    case 'z':

      break;
    case '1':
      font_rect[i].x = 16;
      font_rect[i].y = 16;
      break;
    case '2':
      font_rect[i].x = 32;
      font_rect[i].y = 16;
      break;
    case '3':
      font_rect[i].x = 48;
      font_rect[i].y = 16;
      break;
    case '4':
      font_rect[i].x = 64;
      font_rect[i].y = 16;
      break;
    case '5':
      font_rect[i].x = 80;
      font_rect[i].y = 16;
      break;
    case '6':
      font_rect[i].x = 96;
      font_rect[i].y = 16;
      break;
    case '7':
      font_rect[i].x = 112;
      font_rect[i].y = 16;
      break;
    case '8':
      font_rect[i].x = 128;
      font_rect[i].y = 16;
      break;
    case '9':
      font_rect[i].x = 144;
      font_rect[i].y = 16;
      break;
    case '0':
      font_rect[i].x = 0;
      font_rect[i].y = 16;
      break;
    case '.':

      break;
    case ',':

      break;
    case ':':
      font_rect[i].x = 160;
      font_rect[i].y = 16;
      break;
    case '!':
      font_rect[i].x = 16;
      font_rect[i].y = 0;
      break;
    case '%':

      break;
    case '$':

      break;
    case '&':

      break;
    case '=':

      break;
    case '+':

      break;
    case '-':

      break;
    case '#':

      break;
    }

    SDL_BlitSurface(fontTexture, &font_rect[i], screen, &glyph_pos[i]);
  }
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

		if (enemy_bullets[i]->pos.y >= 240 - 3)
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
		enemy[i]->rect.h = 16;
		enemy[i]->rect.w = 16;
		enemy[i]->rect.x = 0;
		enemy[i]->rect.y = 0;
		enemy[i]->pos.x = itemCount * 30;
		enemy[i]->pos.y = 30 * rowCount;
		enemy[i]->startPos = enemy[i]->pos.x;
		enemy[i]->rowPosID = 30 * (11 - itemCount);
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

		if (i % 5 == 0)
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
		enemy[e]->rect.x = currentFrame * 16;
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

		if (enemy[e]->pos.x >= 560 - (enemy[e]->rect.w + enemy[e]->rowPosID) && enemy[e]->goLeft == 0)
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
	p_move = 400 / 2 - player.tex->w / 2;
	player.pos.y = (240 - 30) - player.tex->h / 2;
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
	else if (player.pos.x >= 400 - player.tex->w)
	{
		player.pos.x = 400 - player.tex->w;
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
		bullets[i]->hitbox.w = 2;
		bullets[i]->hitbox.h = 18;
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
		explo[i]->rect.w = 64;
		explo[i]->rect.h = 64;
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
		explo[e]->rect.x = explo[e]->currentFrame_ex * 64;

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
				addExplo(bullets[i]->pos.x - 64 / 2, bullets[i]->pos.y - 64 / 2);
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
			addExplo(player.pos.x - 64 / 2, player.pos.y - 64 / 2);
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
	p_move = 400 / 2 - player.tex->w / 2;
	player.alive = 1;
}

int main(int argc, char* argv[]) {

	Result rc = romfsInit();
	if (rc)
		printf("romfsInit: %08lX\n", rc);
	else
	{
		printf("romfs Init Successful!\n");
	}

	//Start SDL
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK);
	SDL_ShowCursor(SDL_DISABLE);

	//Set up screen
	//Uint32 flags = SDL_CONSOLEBOTTOM | SDL_TOPSCR;
	Uint32 flags = SDL_DUALSCR;
	screen = SDL_SetVideoMode(400, 480, 16, flags);

	//printf("%i joysticks were found.\n\n", SDL_NumJoysticks());
	printf("The names of the joysticks are:\n");
	for (int i = 0; i < SDL_NumJoysticks(); i++)
	{
		printf("    %s\n", SDL_JoystickName(i));
	}

	SDL_JoystickEventState(SDL_ENABLE);
	joystick = SDL_JoystickOpen(0);

	//Initialize SDL_mixer
	printf("Initialize SDL_mixer\n");
	if (Mix_OpenAudio(22050, AUDIO_S16, 2, 512) == -1)
	{
		return -1;
	}

	load_assets();

	SDL_BlitSurface(fmg_splash_tex2d, NULL, screen, NULL);
	SDL_Flip(screen);
	SDL_Delay(2000);
	SDL_FreeSurface(fmg_splash_tex2d);

	//init stuff
	initEnemies();
	initPlayer();

	//setup fonts and stuff 
  	SDL_Rect textpos;
  	textpos.x = 400 - 200;
  	textpos.y = 10;
  	textpos.w = 0;
 	textpos.h = 0;

	//bottom screen
	SDL_Rect bottom_pos;
	bottom_pos.x = 40;
	bottom_pos.y = 240;

	//load audio
	printf("load audio\n");
	music = Mix_LoadMUS("romfs:/rd/bodenstaendig.ogg");
	snd_blaster = Mix_LoadWAV("romfs:/rd/blaster.ogg");
	snd_explo = Mix_LoadWAV("romfs:/rd/explode1.ogg");
	snd_pusher = Mix_LoadWAV("romfs:/rd/pusher.ogg");

	//Play the music
	printf("play music\n");
	if (Mix_PlayMusic(music, -1) == -1)
	{
		return -1;
	}

	while (quit == 0)
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

				if (e.key.keysym.sym == SDLK_ESCAPE)
				{
					quit = 1;
				}

				if (e.key.keysym.sym == SDLK_RETURN && gameover == 1)
				{
					reset();
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
				printf("key %d\n", e.jbutton.button);

				if (e.jbutton.button == KEY_A && player.alive == 1)
				{
					addBullet(player.pos.x + player.tex->w / 2 - 3, player.pos.y);
					Mix_PlayChannel(-1, snd_blaster, 0);
				}

				if (e.jbutton.button == 11)
				{
					move_right = 1;
				}
				else if (e.jbutton.button == 9)
				{
					move_left = 1;
				}

				if (e.jbutton.button == KEY_START && gameover == 1 || e.jbutton.button == 0 && gameover == 1 )
				{
					reset();
				}
				break;
			case SDL_JOYBUTTONUP:
				if (e.jbutton.button == 11)
				{
					move_right = 0;
				}
				else if (e.jbutton.button == 9)
				{
					move_left = 0;
				}
				break;
			case SDL_JOYHATMOTION:
				// handle hat motion
				LX_HAT= (float)e.jhat.value;
				//printf("LX_HAT: %f\n", LX_HAT);
				if (LX_HAT == 8.0f)
				{
					move_left = 1;
				}
				else
				{
					move_left = 0;
				}

				if (LX_HAT == 2.0f)
				{
					move_right = 1;
				}
				else
				{
					move_right = 0;
				}
				break;
			case SDL_JOYAXISMOTION:
				//printf("Axis Index: %d\n", SDL_JoystickNumAxes(joystick));
				// handle axis motion
				if (e.jaxis.axis == SDL_JoystickGetAxis(joystick, 5))
				{
					LX = (float)e.jaxis.value / GAMEPAD_DEADZONE;
					//printf("LX: %f\n", (float)LX);
				}

				if (LX <= -0.5f)
				{
					move_right = 0;
					move_left = 1;
				}

				if (LX >= 0.5f)
				{
					move_left = 0;
					move_right = 1;
				}

				if (LX < 0.5f && LX > -0.5f)
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
			enemy[e]->rect.x = currentFrame * 16;
		}

		SDL_BlitSurface(space3_tex2d, NULL, screen, NULL);
		SDL_BlitSurface(thumbnail_tex2d, NULL, screen, &bottom_pos); // bottom screen thumbnail

		drawExplo();
		drawEnemies();
		drawBullet();
		drawEnemyBullet();

		if (player.alive == 1) {
			SDL_BlitSurface(player.tex, NULL, screen, &player.pos);
		}
		else
		{
			SDL_BlitSurface(gameover_tex2D, NULL, screen, NULL);
			gameover = 1;
		}

		if (dead_enemies == 15)
		{
			SDL_BlitSurface(win_tex2d, NULL, screen, NULL);
			gameover = 1;
		}

		//printf("MAXCOUNT: %d\n", dead_enemies);

		sprintf(textBuffer, "SCORE:%05d", score);
      	createFont(fontTex, 16, textpos, textBuffer);

		//Update Screen
		SDL_Flip(screen);

		lastTick = (SDL_GetTicks() / 1000);
		SDL_Delay(30);
	}

	SDL_JoystickClose(joystick);
	SDL_FreeSurface(fontTex);
	SDL_FreeSurface(player_tex2d);
	SDL_FreeSurface(bullet_tex2d);
	SDL_FreeSurface(enemy_bullet_tex2d);
	SDL_FreeSurface(explo_tex2d);
	SDL_FreeSurface(gameover_tex2D);
	SDL_FreeSurface(win_tex2d);
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
	romfsExit();

	return 0;
}
