#include <grrlib.h>
#include <ogc/lwp_watchdog.h>   // Needed for gettime and ticks_to_millisecs
#include <stdlib.h>
#include <wiiuse/wpad.h>
#include "oggplayer.h"
#include <fat.h>

// Tile stuff
#define TILE_DELAY  10
#define TILE_UP     12*0
#define TILE_RIGHT  12*1
#define TILE_DOWN   12*2
#define TILE_LEFT   12*3
#define TILE_UP2    12*4+9
#define TILE_RIGHT2 12*5+9
#define TILE_DOWN2  12*6+9
#define TILE_LEFT2  12*7+9

// RGBA Colors
#define GRRLIB_WHITE   0xFFFFFFFF
#define GRRLIB_BLACK   0x000000FF
#define GRRLIB_MAROON  0x800000FF
#define GRRLIB_GREEN   0x008000FF
#define GRRLIB_OLIVE   0x808000FF
#define GRRLIB_NAVY    0x000080FF
#define GRRLIB_PURPLE  0x800080FF
#define GRRLIB_TEAL    0x008080FF
#define GRRLIB_GRAY    0x808080FF
#define GRRLIB_SILVER  0xC0C0C0FF
#define GRRLIB_RED     0xFF0000FF
#define GRRLIB_LIME    0x00FF00FF
#define GRRLIB_YELLOW  0xFFFF00FF
#define GRRLIB_BLUE    0x0000FFFF
#define GRRLIB_FUCHSIA 0xFF00FFFF
#define GRRLIB_AQUA    0x00FFFFFF

static u8 CalculateFrameRate();

// game
struct Rect
{
	int x;
	int y;
	int w;
	int h;
};

struct player_t {
	struct Rect hitbox;
	struct Rect pos;
	GRRLIB_texImg* tex;
	int alive; //bool
};

struct bullet_t
{
	struct Rect hitbox;
	struct Rect pos;
	GRRLIB_texImg* tex;
};

struct enemy_bullet_t
{
    struct Rect hitbox;
	struct Rect pos;
	GRRLIB_texImg* tex;
};

struct explo_t
{
	struct Rect pos;
	struct Rect rect;
	GRRLIB_texImg* tex;
	int animationFinished; //bool
	float currentFrame_ex;
};

struct enemy_t {
	struct Rect hitbox;
	struct Rect pos;
	struct Rect rect;
	GRRLIB_texImg* tex;
	int goLeft; //bool
	int startPos;
	int rowPosID;
	int alive; //bool
	float shootTimer;
	int shoot; //bool
	float shootTimeLimit;
};

//player
#define MAX_BULLETS 50
struct player_t player;
int p_move;

//enemy
#define MAX_ENEMY_BULLETS 100
#define MAX_ENEMIES 40
struct enemy_t* enemy[MAX_ENEMIES] = { NULL };
float currentFrame = 0.0f;

//stuff
struct bullet_t* bullets[MAX_BULLETS] = { NULL };
struct enemy_bullet_t* enemy_bullets[MAX_ENEMY_BULLETS] = { NULL };
#define MAX_EXPLO 100
struct explo_t* explo[MAX_EXPLO] = { NULL };
int rowCount = 0;
int itemCount = 0;
float DeltaTime;
float currentTime;
float lastTick;
int score = 0;
float timer = 0.0f;

GRRLIB_texImg* space3_tex2d;
GRRLIB_texImg* enemy_tex2d;
GRRLIB_texImg* player_tex2d;
GRRLIB_texImg* bullet_tex2d;
GRRLIB_texImg* enemy_bullet_tex2d;
GRRLIB_texImg* explo_tex2d;
GRRLIB_texImg* fmg_splash_tex2d;
GRRLIB_texImg* gameover_tex2D;
GRRLIB_texImg* win_tex2d;
GRRLIB_texImg *scoreFont;

bool quit = false;

u32 wpaddown, wpadheld;

// assets
#include "assets/bullet.h"
#include "assets/enemy_bullet.h"
#include "assets/explode.h"
#include "assets/fmg_splash.h"
#include "assets/gameover.h"
#include "assets/invader32x32x4.h"
#include "assets/player.h"
#include "assets/space3.h"
#include "assets/win.h"
#include "assets/vermin_vibes_1989_24.h"
#include "assets/blaster_ogg.h"
#include "assets/explode1_ogg.h"
#include "assets/pusher_ogg.h"
#include "assets/music_ogg.h"

void load_assets()
{
    space3_tex2d = GRRLIB_LoadTexture(space3_tex);
    enemy_tex2d = GRRLIB_LoadTexture(invader32x32x4_tex);
	GRRLIB_InitTileSet(enemy_tex2d, 32, 32, 0);
    player_tex2d = GRRLIB_LoadTexture(player_tex);
    bullet_tex2d = GRRLIB_LoadTexture(bullet_tex);
    enemy_bullet_tex2d = GRRLIB_LoadTexture(enemy_bullet_tex);
    explo_tex2d = GRRLIB_LoadTexture(explode_tex);
    GRRLIB_InitTileSet(explo_tex2d, 64, 64, 0);
    fmg_splash_tex2d = GRRLIB_LoadTexture(fmg_splash_tex);
    gameover_tex2D = GRRLIB_LoadTexture(gameover_tex);
    win_tex2d = GRRLIB_LoadTexture(win_tex);
    scoreFont = GRRLIB_LoadTexture(vermin_vibes_1989_24);
    GRRLIB_InitTileSet(scoreFont, 24, 24, 32);
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
		enemy_bullets[i] = malloc(sizeof *enemy_bullets[i]);
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
	int i;
	for (i = 0; i < MAX_ENEMY_BULLETS; i++) if (enemy_bullets[i])
	{
		enemy_bullets[i]->pos.y += 5.0f;

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
        GRRLIB_DrawImg(enemy_bullets[i]->pos.x, enemy_bullets[i]->pos.y, enemy_bullets[i]->tex, 0, 1, 1, GRRLIB_WHITE);
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
		enemy[i] = malloc(sizeof *enemy[i]);
		enemy[i]->tex = enemy_tex2d; //invader32x32x4
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
		enemy[i]->shootTimer = 0.0f;
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
	currentFrame +=  0.25f;

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

		enemy[e]->shootTimer += 1.0f/60.0; //1 * DeltaTime;

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
			addEnemyBullet(enemy[e]->pos.x + enemy[e]->rect.w / 2 - 4, enemy[e]->pos.y + 9);
			//Mix_PlayChannel(-1, snd_pusher, 0);
			PlayOgg(pusher_ogg, pusher_ogg_size, 0, OGG_ONE_TIME, 3);
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
            GRRLIB_DrawTile(enemy[e]->pos.x, enemy[e]->pos.y, enemy[e]->tex, 0, 1, 1, GRRLIB_WHITE, (int)currentFrame);
		}
	}
}

/*
   Player
*/

void initPlayer()
{
	player.tex = player_tex2d;
	p_move = 640 / 2 - player.tex->w / 2;
	player.pos.y = (480 - 60) - player.tex->h / 2;
	player.hitbox.w = player.tex->w;
	player.hitbox.h = player.tex->h;
	player.hitbox.x = player.pos.x;
	player.hitbox.y = player.pos.y;
	player.alive = 1;
}

void updatePlayer()
{
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
		bullets[i] = malloc(sizeof *bullets[i]);
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
	int i;
	for (i = 0; i < MAX_BULLETS; i++) if (bullets[i])
	{
		bullets[i]->pos.y -= 5.0f;

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
        GRRLIB_DrawImg(bullets[i]->pos.x, bullets[i]->pos.y, bullets[i]->tex, 0, 1, 1, GRRLIB_WHITE);
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
		explo[i] = malloc(sizeof *explo[i]);
		explo[i]->tex = explo_tex2d;
		explo[i]->pos.x = x;
		explo[i]->pos.y = y;
		explo[i]->rect.w = 128;
		explo[i]->rect.h = 128;
		explo[i]->rect.x = 0;
		explo[i]->rect.y = 0;
		explo[i]->animationFinished = 0;
		explo[i]->currentFrame_ex = 0.0f;
	}
}

void animatorExplo()
{
	int e;
	for (e = 0; e < MAX_EXPLO; e++) if (explo[e]) {
		explo[e]->currentFrame_ex += 0.35f;
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
		GRRLIB_DrawTile(explo[i]->pos.x, explo[i]->pos.y, explo[i]->tex, 0, 2, 2, GRRLIB_WHITE, (int)explo[i]->currentFrame_ex);
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
				//Mix_PlayChannel(0, snd_explo, 0);
				PlayOgg(explode1_ogg, explode1_ogg_size, 0, OGG_ONE_TIME, 2);
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
			WPAD_Rumble(WPAD_CHAN_0, 1); // Rumble on
			addExplo(player.pos.x - 128 / 2, player.pos.y - 128 / 2);
			removeEnemyBullet(i);
			//Mix_PlayChannel(0, snd_explo, 0);
			PlayOgg(explode1_ogg, explode1_ogg_size, 0, OGG_ONE_TIME, 2);
			WPAD_Rumble(WPAD_CHAN_0, 0); // Rumble off
			//printf("BOOM!\n");
			break;
		}
	}

}

void input()
{
	if(wpaddown & WPAD_BUTTON_HOME) {
		quit = true;
    }
	
	if (wpaddown & WPAD_BUTTON_1 && player.alive == 1 || wpaddown & WPAD_BUTTON_2 && player.alive == 1 )
	{
		addBullet(player.pos.x + player.tex->w / 2 - 3, player.pos.y - 9);		
		PlayOgg(blaster_snd, blaster_snd_size, 0, OGG_ONE_TIME, 1);
	}

	if (wpadheld & WPAD_BUTTON_UP) //left
	{
		p_move -= 5.0f;
	}
			 
	if (wpadheld & WPAD_BUTTON_DOWN) //right
	{
		p_move += 5.0f;
	}
}

void reset()
{
	score = 0;
	memset(enemy, 0, sizeof(enemy));
	currentFrame = 0.0f;
	memset(bullets, 0, sizeof(bullets));
	memset(enemy_bullets, 0, sizeof(enemy_bullets));
	memset(explo, 0, sizeof(explo));
	rowCount = 0;
	itemCount = 0;
	initEnemies();
	p_move = 640 / 2 - player.tex->w / 2;
	player.alive = 1;
}

int main(int argc, char* argv[]) {

	unsigned int wait = TILE_DELAY, direction = TILE_DOWN, direction_new = TILE_DOWN;
    u8 FPS = 0;

    ir_t ir1;

	// Initialise the video system
    GRRLIB_Init();

	// Initialise the attached controllers
    WPAD_Init();
    WPAD_SetDataFormat(WPAD_CHAN_0, WPAD_FMT_BTNS_ACC_IR);

	// Initialise the audio subsystem
	ASND_Init();

	load_assets();

	GRRLIB_DrawImg(0, 0, fmg_splash_tex2d, 0, 1, 1, GRRLIB_WHITE);
	GRRLIB_Render();
	sleep(2);
	GRRLIB_FreeTexture(fmg_splash_tex2d);

	//Load image
	space3_tex2d = space3_tex2d;

	//init stuff
	initEnemies();
	initPlayer();

	//TODO: Make better audio implementation
	//Play the music
	//PlayOgg(music_ogg, music_ogg_size, 0, OGG_ONE_TIME, 5);

	while (!quit)
	{
		WPAD_SetVRes(0, 640, 480);
        WPAD_ScanPads();
        wpaddown = WPAD_ButtonsDown(0);
        wpadheld = WPAD_ButtonsHeld(0);

        WPAD_IR(WPAD_CHAN_0, &ir1);

        GRRLIB_FillScreen(GRRLIB_BLACK);    // Clear the screen
        WPAD_Rumble(WPAD_CHAN_0, 0);

		updateExplo();
		updateBullet();
		updateEnemyBullet();
		updateEnemies();

		if (player.alive == 1)
			updatePlayer(); //player

		updateLogic();
		input();

		int e;
		for (e = 0; e < MAX_ENEMIES; e++) if (enemy[e]) {
			enemy[e]->rect.x = currentFrame * 32;
		}

        GRRLIB_DrawImg(0, 0, space3_tex2d, 0, 1, 1, GRRLIB_WHITE);
		drawEnemies();
		drawBullet();
		drawEnemyBullet();
		drawExplo();

		if (player.alive == 1) {
            GRRLIB_DrawImg(player.pos.x, player.pos.y, player.tex, 0, 1, 1, GRRLIB_WHITE);
		}
		else
		{
            GRRLIB_DrawImg(0, 0, gameover_tex2D, 0, 1, 1, GRRLIB_WHITE);

			if (wpaddown & WPAD_BUTTON_PLUS)
			{
				reset();
			}
		}

		if (score >= 4000)
        {
            GRRLIB_DrawImg(0, 0, win_tex2d, 0, 1, 1, GRRLIB_WHITE);

			if (wpaddown & WPAD_BUTTON_PLUS)
			{
				reset();
			}
        }

		char textBuffer[64];
		sprintf(textBuffer, "SCORE:%05d", score);
		GRRLIB_Printf(380, 10, scoreFont, GRRLIB_WHITE, 1, textBuffer);

		GRRLIB_Render();
		FPS = CalculateFrameRate();
	}

	GRRLIB_FreeTexture(space3_tex2d);
	GRRLIB_FreeTexture(enemy_tex2d);
	GRRLIB_FreeTexture(player_tex2d);
	GRRLIB_FreeTexture(bullet_tex2d);
	GRRLIB_FreeTexture(enemy_bullet_tex2d);
	GRRLIB_FreeTexture(explo_tex2d);
	GRRLIB_FreeTexture(fmg_splash_tex2d);
	GRRLIB_FreeTexture(gameover_tex2D);
	GRRLIB_FreeTexture(win_tex2d);
	GRRLIB_FreeTexture(scoreFont);
	memset(enemy, 0, sizeof(enemy));
	memset(bullets, 0, sizeof(bullets));
	memset(enemy_bullets, 0, sizeof(enemy_bullets));
	memset(explo, 0, sizeof(explo));
	GRRLIB_Exit();

	return 0;
}

/**
 * This function calculates the number of frames we render each second.
 * @return The number of frames per second.
 */
static u8 CalculateFrameRate() {
    static u8 frameCount = 0;
    static u32 lastTime;
    static u8 FPS = 0;
    u32 currentTime = ticks_to_millisecs(gettime());
    frameCount++;
	DeltaTime =	(float) (currentTime - lastTick)/1000;
    if(currentTime - lastTime > 1000) {
        lastTime = currentTime;
        FPS = frameCount;
        frameCount = 0;
    }
	lastTick = currentTime;
    return FPS;
}

