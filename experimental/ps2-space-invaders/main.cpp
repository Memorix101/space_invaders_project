#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_image.h>
//PS2SDK
#include <stdio.h> 
#include <tamtypes.h> 
#include <sifrpc.h> 
#include <kernel.h> 
#include <loadfile.h> 
#include <fileio.h> 
#include <string.h> 
#include <errno.h> 
#include <stdlib.h> 
#include <debug.h> 
#include <iopcontrol.h> 

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
	space3_tex2d = IMG_Load("mass:/PS2_SPACE_INVADERS/rd/space3.png");
	enemy_tex2d = IMG_Load("mass:/PS2_SPACE_INVADERS/rd/invader32x32x4.png"); //invader32x32x4
	player_tex2d = IMG_Load("mass:/PS2_SPACE_INVADERS/rd/player.png");
	bullet_tex2d = IMG_Load("mass:/PS2_SPACE_INVADERS/rd/bullet.png");
	enemy_bullet_tex2d = IMG_Load("mass:/PS2_SPACE_INVADERS/rd/enemy-bullet.png");
	explo_tex2d = IMG_Load("mass:/PS2_SPACE_INVADERS/rd/explode.png");
	fmg_splash_tex2d = IMG_Load("mass:/PS2_SPACE_INVADERS/rd/fmg_splash.png");
	gameover_tex2D = IMG_Load("mass:/PS2_SPACE_INVADERS/rd/gameover_ui.png");
	win_tex2d = IMG_Load("mass:/PS2_SPACE_INVADERS/rd/win_ui.png");
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

void GsDmaInit(void)
{
	/*Reset/init dma(gif channel only)*/
	__asm__(
	"li	$2,0x1000A000	\n"
	"nop				\n"
	"sw	$0,0x80($2)		\n"
	"sw	$0,0($2)		\n"
	"sw	$0,0x30($2)		\n"
	"sw	$0,0x10($2)		\n"
	"sw	$0,0x50($2)		\n"
	"sw	$0,0x40($2)		\n"
	"li	$2,0xFF1F		\n"//	0xFF1F
	"sw	$2,0x1000E010	\n"
	"lw	$2,0x1000E010	\n"
	"li	$3,0xFF1F		\n"//0xFF1F
	"and	$2,$3		\n"
	"sw	$2,0x1000E010	\n"
	"sync.p				\n"
	"sw	$0,0x1000E000	\n"
	"sw	$0,0x1000E020	\n"
	"sw	$0,0x1000E030	\n"
	"sw	$0,0x1000E050	\n"
	"sw	$0,0x1000E040	\n"
	"li	$3,1			\n"
	"lw	$2,0x1000E000	\n"
	"ori	$3,$2,1		\n"
	"nop				\n"
	"sw	$3,0x1000E000	\n"
	"nop				\n"
	);
}

void PS2_LoadModules(void)
{
	static char *ModulesList[6] = {
   "rom0:SIO2MAN",
   "rom0:MCMAN",
   "rom0:MCSERV",
   "rom0:PADMAN",
   "rom0:LIBSD",
   "rom0:IOMAN"
	};

	int ret = 0;
	int i;
	for ( i = 0; i < 5; i++ )
	{
		ret = SifLoadModule(ModulesList[i], 0, NULL);
		if ( ret < 0 )
			printf("Failed to load module: %s\n", ModulesList[i]);
	}
}

int main(int argc, char* argv[]) {

	SifInitRpc(0);
    // Reset IOP borrowed from uLaunchelf
    while (!SifIopReset(NULL, 0)){};
    while (!SifIopSync()){};
    SifInitRpc(0);

	GsDmaInit();
	PS2_LoadModules();

	//	Init IOP modules
	//	USB mass support via  IOP modules
	//	The only devices which are loaded by default are 
	//	"mass:" (CD/DVD drive access) and "rom0:" (access to BIOS files). Which are both read only.
	//	http://lukasz.dk/mirror/forums.ps2dev.org/viewtopicb2f6.html?t=11437
	//	So for instance, to access a file in "models/player.obj" 
	//	that is saved into a USB mass storage device you would need to pass the path: "mass:/models/player.obj". 
	//	Some of the prefixes are
	//	"mass:" For the USB mass storage device.
	//	"mc0:" or "mc1:" Memory Cards slot 0 or 1.
	//	"cdfs:" The CD/DVD file system.
	//	"hdd:" The PlayStation 2 compatible external Hard Drive.
	int ret;
	//char freesio2_irx; 
	//int size_freesio2_irx;
	//ret = SifExecModuleBuffer(&freesio2_irx, size_freesio2_irx, 0, NULL, &ret);
	//ret = SifLoadModule("rom0:SIO2MAN", 0, NULL);	
	//if (ret < 0) { 
    //  printf("Error '%d' loading module freesio2\n", ret); 
   	//} else { 
    //  printf("Module freesio2 loaded\n"); 
   	//} 
   	//mcInit(MC_TYPE_MC);*/     
	unsigned char usbd_irx; 
	int size_usbd_irx = 0;
	ret = SifExecModuleBuffer(&usbd_irx, size_usbd_irx, 0, NULL, &ret);
	if (ret < 0) { 
      	printf("Error '%d' loading module usbd.irx\n", ret); 
   	} else { 
    	printf("Module usbd.irx loaded\n"); 
   	}
	unsigned char usbhdfsd_irx; 
	int size_usbhdfsd_irx = 0;
	ret = SifExecModuleBuffer(&usbhdfsd_irx, size_usbhdfsd_irx, 0, NULL, &ret);
	if (ret < 0) { 
    	printf("Error '%d' loading module usbhdfsd.irx\n", ret); 
   	} else { 
    	printf("Module usbhdfsd.irx loaded\n"); 
   	}
	unsigned char usbmass_bd_irx; 
	int size_usbmass_bd_irx = 0;
	ret = SifExecModuleBuffer(&usbmass_bd_irx, size_usbmass_bd_irx, 0, NULL, &ret);
   	if (ret < 0) { 
      printf("Error '%d' loading module usbmass_bd.irx\n", ret); 
   	} else { 
      printf("Module usbmass_bd.irx loaded\n"); 
   	}

	//Start SDL
	if(SDL_Init(SDL_INIT_VIDEO) == -1) //
	{
		printf("Couldn't initialize SDL: %s\n", SDL_GetError());
		return -1;
	}

	SDL_ShowCursor(SDL_DISABLE);

	//Set up screen
	printf("Set up screen\n"); 
	Uint32 flags = SDL_HWSURFACE | SDL_DOUBLEBUF;
	//screen = SDL_SetVideoMode(640, 480, 16, flags);
	screen = SDL_SetVideoMode(640, 480, 16, flags);
	if ( screen == NULL ) {
		fprintf(stderr, "Couldn't set video mode: %s\n",SDL_GetError());
		printf("Couldn't set video mode: %s\n",SDL_GetError()); 
		return -1;
	}

	//printf("%i joysticks were found.\n\n", SDL_NumJoysticks());
	/*printf("The names of the joysticks are: \n");
	for (int i = 0; i < SDL_NumJoysticks(); i++)
	{
		printf("    %s\n", SDL_JoystickName(i));
	}

	SDL_JoystickEventState(SDL_ENABLE);
	joystick = SDL_JoystickOpen(0);*/

	printf("TTF Init\n"); 
	if (TTF_Init() == -1)
	{
		printf("SDL_ttf error\n"); 
		return -1;
	}


	//Initialize SDL_mixer
	//printf("Initialize SDL_mixer\n"); 
	/*if (Mix_OpenAudio(44100, AUDIO_S16, 2, 512) == -1)
	{
		printf("SDL_mixer error\n"); 
		return -1;
	}*/
	
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
	TTF_Font* vermin_ttf = TTF_OpenFont("mass:/PS2_SPACE_INVADERS/rd/vermin_vibes_1989.ttf", 24);
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
	//music = Mix_LoadMUS("mass:/PS2_SPACE_INVADERS/rd/bodenstaendig.ogg");
	//snd_blaster = Mix_LoadWAV("mass:/PS2_SPACE_INVADERS/rd/blaster.ogg");
	//snd_explo = Mix_LoadWAV("mass:/PS2_SPACE_INVADERS/rd/explode1.ogg");
	//snd_pusher = Mix_LoadWAV("mass:/PS2_SPACE_INVADERS/rd/pusher.ogg");

	//Play the music
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
				if (e.jbutton.button == 1 && player.alive == 1)
				{
					addBullet(player.pos.x + player.tex->w / 2 - 3, player.pos.y);
					Mix_PlayChannel(-1, snd_blaster, 0);
				}

				if (e.jbutton.button == 5 && gameover == 1)
				{
					reset();
				}

				if (e.jbutton.button == 4)
				{
					quit = 1;
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

		printf("zoom!!!\n"); 

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

		lastTick = (SDL_GetTicks() / 1000);
		SDL_Delay(30);
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