#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_endian.h>
#include <SDL/SDL_mixer.h>

#include <tamtypes.h>
#include <kernel.h>
#include <loadfile.h>
#include <stdio.h>
#include <sifrpc.h>
#include <gs_privileged.h>
#include <libpad.h>
#include <string.h>
#include <cstdlib>
#include <audsrv.h>


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
SDL_Surface* gWindow = NULL;

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
int gameover = 0;
int quit = 0;
int dead_enemies = 0;
bool move_left = 0;
bool move_right = 0;

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

// controller
static char padBuf[256] __attribute__((aligned(64)));
static char actAlign[6];
static int actuators;

// audio
Mix_Music *music = NULL;
Mix_Chunk *snd_pusher = NULL;
Mix_Chunk *snd_blaster = NULL;
Mix_Chunk *snd_explo = NULL;

//time
float Timestep = 0;
float lastdelta = 0;
float curTime = 0;
float prewarm = 0;

// workaround http://lukasz.dk/mirror/forums.ps2dev.org/viewtopicb2f6.html?t=11437
SDL_RWops *rwop;
char filename[50];

void load_assets()
{
	sprintf(filename, "host:rd/space3.png");
	rwop = SDL_RWFromFile(filename, "rb");
	space3_tex2d = IMG_LoadPNG_RW(rwop);

	sprintf(filename, "host:rd/invader32x32x4.png");
	rwop = SDL_RWFromFile(filename, "rb");
	enemy_tex2d = IMG_LoadPNG_RW(rwop); //invader32x32x4
	
	sprintf(filename, "host:rd/player.png");
	rwop = SDL_RWFromFile(filename, "rb");
	player_tex2d = IMG_LoadPNG_RW(rwop);

	sprintf(filename, "host:rd/bullet.png");
	rwop = SDL_RWFromFile(filename, "rb");
	bullet_tex2d = IMG_LoadPNG_RW(rwop);

	sprintf(filename, "host:rd/enemy-bullet.png");
	rwop = SDL_RWFromFile(filename, "rb");
	enemy_bullet_tex2d = IMG_LoadPNG_RW(rwop);

	sprintf(filename, "host:rd/explode.png");
	rwop = SDL_RWFromFile(filename, "rb");
	explo_tex2d = IMG_LoadPNG_RW(rwop);

	sprintf(filename, "host:rd/fmg_splash.png");
	rwop = SDL_RWFromFile(filename, "rb");
	fmg_splash_tex2d = IMG_LoadPNG_RW(rwop);

	sprintf(filename, "host:rd/gameover_ui.png");
	rwop = SDL_RWFromFile(filename, "rb");
	gameover_tex2D = IMG_LoadPNG_RW(rwop);

	sprintf(filename, "host:rd/win_ui.png");
	rwop = SDL_RWFromFile(filename, "rb");
	win_tex2d = IMG_LoadPNG_RW(rwop);
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
		printf("shootTimeLimit %d\n", enemy[i]->shootTimeLimit);
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

		if (enemy[e]->pos.x >= 640 - (enemy[e]->rect.w + enemy[e]->rowPosID) && enemy[e]->goLeft == 0)
		{
			enemy[e]->goLeft = 1;
		}

		if (enemy[e]->pos.x <= enemy[e]->startPos + enemy[e]->rect.w && enemy[e]->goLeft == 1)
		{
			enemy[e]->goLeft = 0;
		}

		enemy[e]->shootTimer += 1 * DeltaTime;

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
	p_move = 640 / 2 - player.tex->w / 2;
	player.alive = 1;
}

static void loadModules(void)
{
    int ret;

    ret = SifLoadModule("rom0:SIO2MAN", 0, NULL);
    if (ret < 0) {
        printf("sifLoadModule sio failed: %d\n", ret);
        SleepThread();
    }

    ret = SifLoadModule("rom0:PADMAN", 0, NULL);
    if (ret < 0) {
        printf("sifLoadModule pad failed: %d\n", ret);
        SleepThread();
    }    
	
	ret = SifLoadModule("rom0:LIBSD", 0, NULL);
    if (ret < 0) {
        printf("sifLoadModule libsd failed: %d\n", ret);
        SleepThread();
    }	

	ret = SifLoadModule("rom0:MCMAN", 0, NULL);
    if (ret < 0) {
        printf("sifLoadModule MCMAN failed: %d\n", ret);
        SleepThread();
    }		

	/*ret = SifLoadModule("rom0:ADDDRV", 0, NULL);
    if (ret < 0) {
        printf("sifLoadModule ADDDRV failed: %d\n", ret);
        SleepThread();
    }			

	ret = SifLoadModule("rom0:ROMDRV", 0, NULL);
    if (ret < 0) {
        printf("sifLoadModule ROMDRV failed: %d\n", ret);
        SleepThread();
    }

	ret = SifLoadModule("rom0:CDVDFSV", 0, NULL);
    if (ret < 0) {
        printf("sifLoadModule CDVDFSV failed: %d\n", ret);
        SleepThread();
    }	

	ret = SifLoadModule("rom0:CDVDMAN", 0, NULL);
    if (ret < 0) {
        printf("sifLoadModule CDVDMAN failed: %d\n", ret);
        SleepThread();
    }	*/

	ret = SifLoadModule("host:audsrv.irx", 0, NULL);
    if (ret < 0) {
        printf("sifLoadModule audsrv failed: %d\n", ret);
        SleepThread();
    }

	/*ret = SifLoadModule("host:usbd.irx", 0, 0);
    if (ret < 0) {
        printf("sifLoadModule usbd failed: %d\n", ret);
        SleepThread();
    }

	ret = SifLoadModule("host:usbhdfsd.irx", 0, 0);
    if (ret < 0) {
        printf("sifLoadModule usbhdfsd failed: %d\n", ret);
        SleepThread();
    }*/
}

static int waitPadReady(int port, int slot)
{
    int state;
    int lastState;
    char stateString[16];

    state = padGetState(port, slot);
    lastState = -1;
    while((state != PAD_STATE_STABLE) && (state != PAD_STATE_FINDCTP1)) {
        if (state != lastState) {
            padStateInt2String(state, stateString);
            printf("Please wait, pad(%d,%d) is in state %s\n",
                       port, slot, stateString);
        }
        lastState = state;
        state=padGetState(port, slot);
    }
    // Were the pad ever 'out of sync'?
    if (lastState != -1) {
        printf("Pad OK!\n");
    }
    return 0;
}

static int initializePad(int port, int slot)
{
    int ret;
    int modes;
    int i;

    waitPadReady(port, slot);

    // How many different modes can this device operate in?
    // i.e. get # entrys in the modetable
    modes = padInfoMode(port, slot, PAD_MODETABLE, -1);
    printf("The device has %d modes\n", modes);

    if (modes > 0) {
        printf("( ");
        for (i = 0; i < modes; i++) {
            printf("%d ", padInfoMode(port, slot, PAD_MODETABLE, i));
        }
        printf(")");
    }

    printf("It is currently using mode %d\n",
               padInfoMode(port, slot, PAD_MODECURID, 0));

    // If modes == 0, this is not a Dual shock controller
    // (it has no actuator engines)
    if (modes == 0) {
        printf("This is a digital controller?\n");
        return 1;
    }

    // Verify that the controller has a DUAL SHOCK mode
    i = 0;
    do {
        if (padInfoMode(port, slot, PAD_MODETABLE, i) == PAD_TYPE_DUALSHOCK)
            break;
        i++;
    } while (i < modes);
    if (i >= modes) {
        printf("This is no Dual Shock controller\n");
        return 1;
    }

    // If ExId != 0x0 => This controller has actuator engines
    // This check should always pass if the Dual Shock test above passed
    ret = padInfoMode(port, slot, PAD_MODECUREXID, 0);
    if (ret == 0) {
        printf("This is no Dual Shock controller??\n");
        return 1;
    }

    printf("Enabling dual shock functions\n");

    // When using MMODE_LOCK, user cant change mode with Select button
    padSetMainMode(port, slot, PAD_MMODE_DUALSHOCK, PAD_MMODE_LOCK);

    waitPadReady(port, slot);
    printf("infoPressMode: %d\n", padInfoPressMode(port, slot));

    waitPadReady(port, slot);
    printf("enterPressMode: %d\n", padEnterPressMode(port, slot));

    waitPadReady(port, slot);
    actuators = padInfoAct(port, slot, -1, 0);
    printf("# of actuators: %d\n",actuators);

    if (actuators != 0) {
        actAlign[0] = 0;   // Enable small engine
        actAlign[1] = 1;   // Enable big engine
        actAlign[2] = 0xff;
        actAlign[3] = 0xff;
        actAlign[4] = 0xff;
        actAlign[5] = 0xff;

        waitPadReady(port, slot);
        printf("padSetActAlign: %d\n",
                   padSetActAlign(port, slot, actAlign));
    }
    else {
        printf("Did not find any actuators.\n");
    }

    waitPadReady(port, slot);

    return 1;
}


int main(int argc, char *argv[]) {

    int ret;
    int port, slot;
    int ctr;
    struct padButtonStatus buttons;
    u32 paddata;
    u32 old_pad = 0;
    u32 new_pad;

	SifInitRpc(0);

    loadModules();
    padInit(0);
    port = 0; // 0 -> Connector 1, 1 -> Connector 2
    slot = 0; // Always zero if not using multitap
    printf("PortMax: %d\n", padGetPortMax());
    printf("SlotMax: %d\n", padGetSlotMax(port));

    if((ret = padPortOpen(port, slot, padBuf)) == 0) {
        printf("padOpenPort failed: %d\n", ret);
        SleepThread();
    }

    if(!initializePad(port, slot)) {
        printf("pad initalization failed!\n");
        SleepThread();
    }

	//Start SDL
	printf("Set up SDL\n"); 
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO) < 0) //
	{
		printf("Couldn't initialize SDL: %s\n", SDL_GetError());
		return -1;
	}

	SDL_ShowCursor(SDL_DISABLE);

	//Set up screen
	printf("Set up screen\n"); 
	Uint32 flags = SDL_HWSURFACE | SDL_DOUBLEBUF;
	//screen = SDL_SetVideoMode(640, 480, 16, flags);
	screen = SDL_SetVideoMode(640, 480, 16, 0);
	if ( screen == NULL ) {
		fprintf(stderr, "Couldn't set video mode: %s\n",SDL_GetError());
		printf("Couldn't set video mode: %s\n",SDL_GetError()); 
		return -1;
	}

	printf("TTF Init\n"); 
	if (TTF_Init() < 0)
	{
		printf("SDL_ttf error\n"); 
		return -1;
	}

	//Initialize audio
	printf("Initialize SDL_mixer\n"); 
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 512) < 0) 
	{
		printf("Couldn't open audio: %s\n", SDL_GetError());
		return 0;
	}

	printf("Loading assets ... \n"); 
	load_assets();

	printf("Displaying splash screen... \n"); 
	SDL_BlitSurface(fmg_splash_tex2d, NULL, screen, NULL);
	SDL_Flip(screen);
	SDL_Delay(2000);
	SDL_FreeSurface(fmg_splash_tex2d);

	//init stuff
	initEnemies();
	initPlayer();

	//ttf stuff
	sprintf(filename, "host:rd/vermin_vibes_1989.ttf");
	rwop = SDL_RWFromFile(filename, "rb");
	TTF_Font* vermin_ttf = TTF_OpenFontRW(rwop, 1, 24);

	SDL_Rect score_pos;
	SDL_Color Color = { 255, 255, 255 };
	char textBuffer[64];
	sprintf(textBuffer, "SCORE: % 05d", score);
	SDL_Surface* scoreText = TTF_RenderText_Solid(vermin_ttf, textBuffer, Color);
	score_pos.x = 640 - scoreText->w - 10;
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
	music = Mix_LoadMUS("host:rd/bodenstaendig.wav");

	sprintf(filename, "host:rd/blaster.wav");
	rwop = SDL_RWFromFile(filename, "rb");
	snd_blaster = Mix_LoadWAV_RW(rwop, 1);

	sprintf(filename, "host:rd/explode1.wav");
	rwop = SDL_RWFromFile(filename, "rb");
	snd_explo = Mix_LoadWAV_RW(rwop, 1);

	sprintf(filename, "host:rd/pusher.wav");
	rwop = SDL_RWFromFile(filename, "rb");
	snd_pusher = Mix_LoadWAV_RW(rwop, 1);

	//Play the music
	if (Mix_PlayMusic(music, -1) == -1)
	{
		return -1;
	}

	while (quit == 0)
	{
		curTime = (float)SDL_GetTicks()/1000.0f;
    	DeltaTime = curTime - lastdelta;

		/*printf("shootTimer %d\n", enemy[0]->shootTimer);
		printf("shootTimeLimit %d\n", enemy[0]->shootTimeLimit);*/
		
		//Handle controller input
        ctr = 0;
        ret = padGetState(port, slot);
        while((ret != PAD_STATE_STABLE) && (ret != PAD_STATE_FINDCTP1)) {
            if(ret==PAD_STATE_DISCONN) {
                printf("Pad(%d, %d) is disconnected\n", port, slot);
            }
            ret = padGetState(port, slot);
        }
        if(ctr==1) {
            printf("Pad: OK!\n");
        }

        ret = padRead(port, slot, &buttons); // port, slot, buttons

        if (ret != 0) {
            paddata = 0xffff ^ buttons.btns;

            new_pad = paddata & ~old_pad;
            old_pad = paddata;

            // Directions
            if(new_pad & PAD_LEFT) {
                //printf("LEFT\n");
            }

			if(old_pad & PAD_LEFT) {
                //printf("old_pad LEFT\n");
				move_left = 1;
            }
			else
			{
				move_left = 0;
			}

            if(new_pad & PAD_DOWN) {
                //printf("DOWN\n");
            }

            if(new_pad & PAD_RIGHT) {
                //printf("RIGHT\n");
                /*
                       padSetMainMode(port, slot,PAD_MMODE_DIGITAL, PAD_MMODE_LOCK));
                */
            }

			if(old_pad & PAD_RIGHT) {
                //printf("old_pad RIGHT\n");
				move_right = 1;
                /*
                       padSetMainMode(port, slot,PAD_MMODE_DIGITAL, PAD_MMODE_LOCK));
                */
            }
			else
			{
				move_right = 0;
			}

			if(buttons.ljoy_h > 240)
            {
				move_right = 1;
            }
			else if(old_pad & !PAD_RIGHT)
			{
				move_right = 0;
			}

			//printf("%d\n", (int)buttons.ljoy_h);
			if(buttons.ljoy_h < 64)
            {
				move_left = 1;
            }
			else if(old_pad & !PAD_LEFT)
			{
				move_left = 0;
			}

            if(new_pad & PAD_UP) {
                //printf("UP\n");
            }
            if(new_pad & PAD_START && gameover == 1) {
                //printf("START\n");
				reset();
            }
            if(new_pad & PAD_R3) {
                //printf("R3\n");
            }
            if(new_pad & PAD_L3) {
                //printf("L3\n");
            }
            if(new_pad & PAD_SELECT) {
                //printf("SELECT\n");
            }
            if(new_pad & PAD_SQUARE) {
                //printf("SQUARE\n");
            }
            if(new_pad & PAD_CROSS && player.alive == 1) {
                //padEnterPressMode(port, slot);
                //printf("CROSS - Enter press mode\n");
				addBullet(player.pos.x + player.tex->w / 2 - 3, player.pos.y);
            }
            if(new_pad & PAD_CIRCLE) {
                //padExitPressMode(port, slot);
                //printf("CIRCLE - Exit press mode\n");
            }
            if(new_pad & PAD_TRIANGLE) {
                // Check for the reason below..
                //printf("TRIANGLE (press mode disabled, see code)\n");
            }
            if(new_pad & PAD_R1 && player.alive == 1) {
                /*actAlign[0] = 1; // Start small engine
                padSetActDirect(port, slot, actAlign);
                printf("R1 - Start little engine\n");*/
				addBullet(player.pos.x + player.tex->w / 2 - 3, player.pos.y);
            }
            if(new_pad & PAD_L1) {
                /*actAlign[0] = 0; // Stop engine 0
                padSetActDirect(port, slot, actAlign);
                printf("L1 - Stop little engine\n");*/
            }
            if(new_pad & PAD_R2) {
                //printf("R2\n");
            }
            if(new_pad & PAD_L2) {
                //printf("L2\n");
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
		SDL_Flip(screen);

		lastdelta = curTime;
		//SDL_Delay(30);
	}

	SDL_JoystickClose(joystick);
	SDL_FreeSurface(youWin);
	SDL_FreeSurface(gameOver);
	SDL_FreeSurface(player_tex2d);
	SDL_FreeSurface(bullet_tex2d);
	SDL_FreeSurface(enemy_bullet_tex2d);
	SDL_FreeSurface(explo_tex2d);
	SDL_FreeSurface(gameover_tex2D);
	SDL_FreeSurface(win_tex2d);
	TTF_CloseFont(vermin_ttf);
	Mix_FreeMusic(music);
	Mix_FreeChunk(snd_blaster);
	Mix_FreeChunk(snd_explo);
	Mix_FreeChunk(snd_pusher);
	audsrv_quit();
	SDL_RWclose(rwop);
	memset(enemy, 0, sizeof(enemy));
	memset(bullets, 0, sizeof(bullets));
	memset(enemy_bullets, 0, sizeof(enemy_bullets));
	memset(explo, 0, sizeof(explo));

	//Quit SDL
	SDL_Quit();

	return 0;
}
