#include <cstdlib>
#include <stdio.h>
#include <string>
#include <errno.h>

#include <switch.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>


struct player_t {
	SDL_Rect hitbox;
	SDL_Rect pos;
	SDL_Texture* tex;
	SDL_Rect size;
	SDL_Rect vec; //representing pos and size
	int alive; //bool
};

struct bullet_t
{
	SDL_Rect hitbox;
	SDL_Rect pos;
	SDL_Texture* tex;
	SDL_Rect size;
	SDL_Rect vec; //representing pos and size
};

struct enemy_bullet_t
{
	SDL_Rect hitbox;
	SDL_Rect pos;
	SDL_Texture* tex;
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
	int currentFrame_ex;
	SDL_Rect spritecut;
};

struct enemy_t {
	SDL_Rect hitbox;
	SDL_Rect pos;
	SDL_Rect rect;
	SDL_Texture* tex;
	SDL_Rect size;
	int goLeft; //bool
	int startPos;
	int rowPosID;
	int alive; //bool
	float shootTimer;
	int shoot; //bool
	float shootTimeLimit;
	SDL_Rect vec; //representing pos and size
	SDL_Rect spritecut;
};

//base SDL stuff
SDL_Event e;
SDL_Renderer* renderer = NULL;
SDL_Window* window = NULL;

SDL_Joystick* joystick = NULL;
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
	space3_surface = IMG_Load("romfs:/rd/space3.png");
	space3_tex = SDL_CreateTextureFromSurface(renderer, space3_surface);
	SDL_FreeSurface(space3_surface);

	enemy_surface = IMG_Load("romfs:/rd/invader32x32x4.png"); //invader32x32x4
	enemy_tex = SDL_CreateTextureFromSurface(renderer, enemy_surface);

	player_surface = IMG_Load("romfs:/rd/player.png");
	player_tex = SDL_CreateTextureFromSurface(renderer, player_surface);

	bullet_surface = IMG_Load("romfs:/rd/bullet.png");
	bullet_tex = SDL_CreateTextureFromSurface(renderer, bullet_surface);

	enemy_bullet_surface = IMG_Load("romfs:/rd/enemy-bullet.png");
	enemy_bullet_tex = SDL_CreateTextureFromSurface(renderer, enemy_bullet_surface);

	explo_surface = IMG_Load("romfs:/rd/explode.png");
	explo_tex = SDL_CreateTextureFromSurface(renderer, explo_surface);

	fmg_splash_surface = IMG_Load("romfs:/rd/fmg_splash.png");
	fmg_splash_tex = SDL_CreateTextureFromSurface(renderer, fmg_splash_surface);
	SDL_FreeSurface(fmg_splash_surface);

	gameover_surface = IMG_Load("romfs:/rd/gameover_ui.png");
	gameover_tex = SDL_CreateTextureFromSurface(renderer, gameover_surface);
	SDL_FreeSurface(gameover_surface);

	win_surface = IMG_Load("romfs:/rd/win_ui.png");
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
		//printf("Pew Pew!");
		int i = found;
		enemy_bullets[i] = (enemy_bullet_t*)malloc(sizeof(enemy_bullet_t));
		enemy_bullets[i]->tex = enemy_bullet_tex;
		enemy_bullets[i]->pos = {(int)x, (int)y};
		enemy_bullets[i]->size = { 0, 0, enemy_bullet_surface->w, enemy_bullet_surface->h};
		enemy_bullets[i]->hitbox = { 0, 0, enemy_bullets[i]->size.w, enemy_bullets[i]->size.h};
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
		enemy[i]->tex = enemy_tex;
		enemy[i]->alive = 1;
		enemy[i]->rect = {0,0,32, 32};
		enemy[i]->pos = { itemCount * 40, 40 * rowCount };
		enemy[i]->startPos = enemy[i]->pos.x;
		enemy[i]->rowPosID = 40 * (11 - itemCount);
		enemy[i]->goLeft = 0;
		enemy[i]->hitbox = { enemy[i]->pos.x, enemy[i]->pos.y, enemy[i]->rect.w, enemy[i]->rect.h };
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

		enemy[e]->vec = { enemy[e]->pos.x, enemy[e]->pos.y, enemy[e]->size.w, enemy[e]->size.h };
		enemy[e]->spritecut = { enemy[e]->pos.x, enemy[e]->pos.y, enemy[e]->rect.w, enemy[e]->rect.h };
	}
}

void drawEnemies()
{
	for (int e = 0; e < MAX_ENEMIES; e++) if (enemy[e]) {
		if (enemy[e]->alive == 1) {
			//SDL_BlitSurface(enemy[e]->tex, &enemy[e]->rect, screen, &enemy[e]->pos);
			SDL_RenderCopy(renderer, enemy[e]->tex, &enemy[e]->rect, &enemy[e]->spritecut);
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

//Player
void initPlayer()
{
	player.tex = player_tex;
	p_move = 640 / 2 - player_surface->w / 2;
	player.pos.y = (480 - 60) - player_surface->h / 2;
	player.size = {0, 0,player_surface->w, player_surface->h };
	player.size = { player.pos.x, player.pos.y, player.size.w, player.size.h };
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

	if (player.pos.x <= 0)
	{
		player.pos.x = 0;
	}
	else if (player.pos.x >= 640 - player.size.w)
	{
		player.pos.x = 640 - player.size.w;
	}

	player.hitbox = { player.pos.x, player.pos.y, player.size.w, player.size.h };
	player.vec = { player.pos.x, player.pos.y, player.size.w, player.size.h };
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
		bullets[i]->pos = { (int)x, (int)y };
		bullets[i]->size = {0, 0,bullet_surface->w, bullet_surface->h };
		bullets[i]->hitbox = {0, 0,bullets[i]->size.w, bullets[i]->size.h };
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
		explo[i]->pos = {(int)x, (int)y};
		explo[i]->size = { 0, 0, explo_surface->w, explo_surface->h };
		explo[i]->rect = { 0, 0,128,128 };
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
		explo[i]->vec = { explo[i]->pos.x, explo[i]->pos.y, 0, 0 };
		explo[i]->spritecut = {explo[i]->pos.x, explo[i]->pos.y, explo[i]->rect.w, explo[i]->rect.h };
		
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

 //Etc

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
	p_move = 640 / 2 - player.size.w / 2;
	player.alive = 1;
}

int main(int argc, char* argv[]) {

	romfsInit();

	SDL_Init(SDL_INIT_EVERYTHING);

	window = SDL_CreateWindow(nullptr, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1920, 1080, SDL_WINDOW_FULLSCREEN_DESKTOP); //SDL_WINDOW_FULLSCREEN
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	SDL_RenderSetLogicalSize(renderer, 640, 480);

	//Initialize SDL_mixer
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 4096) == -1)
	{
		return false;
	}

	//Set up TTf stuff
	if (TTF_Init() == -1)
	{
		return false;
	}

	//printf("%i joysticks were found.\n\n", SDL_NumJoysticks());
	/*printf("The names of the joysticks are:\n");
	for (int i = 0; i < SDL_NumJoysticks(); i++)
	{
		printf("    %s\n", SDL_JoystickNameForIndex(i)); //SDL_NumJoysticks
	}*/

	SDL_JoystickEventState(SDL_ENABLE);
	joystick = SDL_JoystickOpen(0);

	load_assets();
	music = Mix_LoadMUS("romfs:/rd/bodenstaendig.ogg");

	SDL_RenderCopy(renderer, fmg_splash_tex, NULL, NULL);
	SDL_RenderPresent(renderer);
	SDL_Delay(2000);

	//init stuff
	initEnemies();
	initPlayer();

	vermin_ttf = TTF_OpenFont("romfs:/rd/vermin_vibes_1989.ttf", 24);
	char textBuffer[64];
	sprintf(textBuffer, "SCORE: % 05d", score);
	scoreText = TTF_RenderText_Solid(vermin_ttf, textBuffer, { 255, 255, 255 });
	font_tex = SDL_CreateTextureFromSurface(renderer, scoreText);
	score_pos = { 640 - scoreText->w - 10, 10 };
	score_tex_pos = { score_pos.x, score_pos.y, scoreText->w, scoreText->h };
	SDL_FreeSurface(scoreText);

	//load audio
	snd_blaster = Mix_LoadWAV("romfs:/rd/blaster.ogg");
	snd_explo = Mix_LoadWAV("romfs:/rd/explode1.ogg");
	snd_pusher = Mix_LoadWAV("romfs:/rd/pusher.ogg");

	//Play the music
	if (Mix_PlayMusic(music, -1) == -1)
	{
		return 1;
	}

	while (quit == 0)
	{
		DeltaTime = ((float)SDL_GetTicks() / 1000.0f) - lastTick;

		//Handle events on queue
		while (SDL_PollEvent(&e) != 0)
		{
			switch (e.type)
			{
			case SDL_KEYDOWN:
				if (e.key.keysym.sym == SDLK_SPACE && player.alive == 1)
				{
					addBullet(player.pos.x + player.size.w / 2 - 3, player.pos.y);
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
			case SDL_JOYBUTTONDOWN:
				if (e.jbutton.button == 0 && player.alive || e.jbutton.button == 1 && player.alive ||
					e.jbutton.button == 3 && player.alive || e.jbutton.button == 7 && player.alive || 
					e.jbutton.button == 9 && player.alive)
				{
					addBullet(player.pos.x + player.size.w / 2 - 3, player.size.y);
					Mix_PlayChannel(-1, snd_blaster, 0);
				}

				if (e.jbutton.button == 10 && gameover == 1)
				{
					reset();
				}

				if (e.jbutton.button == 12 && e.jbutton.state == SDL_PRESSED)
				{
					move_left = 1;
				}		

				if (e.jbutton.button == 14 && e.jbutton.state == SDL_PRESSED)
				{
					move_right = 1;
				}	

				if (e.jbutton.button == 11)
				{
					quit = 1;
				}
				break;
			case SDL_JOYBUTTONUP:
				if (e.jbutton.button == 12 && e.jbutton.state == SDL_RELEASED)
				{
					move_left = 0;
				}

				if (e.jbutton.button == 14 && e.jbutton.state == SDL_RELEASED)
				{
					move_right = 0;
				}	
				break;
			case SDL_JOYAXISMOTION:
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

		//draw stuff
		SDL_RenderClear(renderer);

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

		SDL_RenderCopy(renderer, space3_tex, NULL, NULL);

		drawExplo();
		drawEnemies();
		drawBullet();
		drawEnemyBullet();

		if (player.alive == 1) {
			SDL_RenderCopy(renderer, player.tex, NULL, &player.vec);
		}
		else
		{
			SDL_RenderCopy(renderer, gameover_tex, NULL, NULL);
			gameover = 1;
		}

		if (dead_enemies == 40)
		{
			SDL_RenderCopy(renderer, win_tex, NULL, NULL);
			gameover = 1;
		}


		//printf("MAXCOUNT: %d\n", dead_enemies);

		//this ugly block is updating the score
		sprintf(textBuffer, "SCORE: % 05d", score);
		SDL_RenderCopy(renderer, font_tex, NULL, &score_tex_pos);
		scoreText = NULL;
		scoreText = TTF_RenderText_Solid(vermin_ttf, textBuffer, { 255, 255, 255 });
		font_tex = SDL_CreateTextureFromSurface(renderer, scoreText);
		score_pos = { 640 - scoreText->w - 10, 10 };
		score_tex_pos = { score_pos.x, score_pos.y, scoreText->w, scoreText->h };
		SDL_FreeSurface(scoreText);


		//Update Screen
		SDL_RenderPresent(renderer);

		lastTick = (float)SDL_GetTicks() / 1000.0f;
		SDL_Delay(30);
	}

	SDL_JoystickClose(joystick);
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
	romfsExit();

	return 0;
}