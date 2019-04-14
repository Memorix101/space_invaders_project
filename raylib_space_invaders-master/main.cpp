#include <cstdlib>
#include <thread>
#include <time.h>
#include <stdio.h>
#include <string>
#include <raylib.h>

struct player_t {
	Rectangle hitbox;
	Rectangle pos;
	Texture2D tex;
	int alive; //bool
};

struct bullet_t
{
	Rectangle hitbox;
	Rectangle pos;
	Texture2D tex;
};

struct enemy_bullet_t
{
	Rectangle hitbox;
	Rectangle pos;
	Texture2D tex;
};

struct explo_t
{
	Rectangle pos;
	Rectangle rect;
	Texture2D tex;
	int animationFinished; //bool
	int currentFrame_ex;
};

struct enemy_t {
	Rectangle hitbox;
	Rectangle pos;
	Rectangle rect;
	Texture2D tex;
	int goLeft; //bool
	int startPos;
	int rowPosID;
	int alive; //bool
	float shootTimer;
	int shoot; //bool
	float shootTimeLimit;
};

//base stuff
Texture2D space;
Texture2D screen;

//player
#define MAX_BULLETS 50
struct player_t player;
int p_move;

//enemy
#define MAX_ENEMY_BULLETS 100
#define MAX_ENEMIES 40
struct enemy_t* enemy[MAX_ENEMIES] = { NULL };
int currentFrame = 0;
int enemies_killed = MAX_ENEMIES;

//stuff
struct bullet_t* bullets[MAX_BULLETS] = { NULL };
struct enemy_bullet_t* enemy_bullets[MAX_ENEMY_BULLETS] = { NULL };
#define MAX_EXPLO 100
struct explo_t* explo[MAX_EXPLO] = { NULL };
int rowCount = 0;
int itemCount = 0;
double DeltaTime;
double lastTick;
int score = 0;
int gameover = 0;

//Music
Music music;
Sound snd_pusher;
Sound snd_blaster;
Sound snd_explo;

Texture2D enemy_tex;
Texture2D player_tex;
Texture2D bullet_tex;
Texture2D enemy_bullet_tex;
Texture2D explo_tex;
Texture2D fmg_tex;
Texture2D space_tex;
Texture2D gameover_tex;
Texture2D win_tex;

SpriteFont vermin_ttf;

void preload_assets()
{
	//load tex
	fmg_tex = LoadTexture("rd/fmg_splash.png");
	space_tex = LoadTexture("rd/space3.png");
	enemy_bullet_tex = LoadTexture("rd/enemy-bullet.png");
	enemy_tex = LoadTexture("rd/invader32x32x4.png");
	bullet_tex = LoadTexture("rd/bullet.png");
	player_tex = LoadTexture("rd/player.png");
	explo_tex = LoadTexture("rd/explode.png");
	gameover_tex = LoadTexture("rd/gameover_ui.png");
	win_tex = LoadTexture("rd/win_ui.png");

	//load audio
	music = LoadMusicStream("rd/bodenstaendig.ogg");
	snd_blaster = LoadSound("rd/blaster.ogg");
	snd_explo = LoadSound("rd/explode1.wav");
	snd_pusher = LoadSound("rd/pusher.wav");

	//load font
	vermin_ttf = LoadFontEx("rd/vermin_vibes_1989.ttf", 24, 0, 0);
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
		enemy_bullets[i]->tex = enemy_bullet_tex;
		enemy_bullets[i]->pos.x = x;
		enemy_bullets[i]->pos.y = y;
		enemy_bullets[i]->hitbox.width = enemy_bullets[i]->tex.width;
		enemy_bullets[i]->hitbox.height = enemy_bullets[i]->tex.height;
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
		DrawTexture(enemy_bullets[i]->tex, enemy_bullets[i]->pos.x, enemy_bullets[i]->pos.y, WHITE);
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
		enemy[i]->tex = enemy_tex; //invader32x32x4
		enemy[i]->alive = 1;
		enemy[i]->rect.height = 32;
		enemy[i]->rect.width = 32;
		enemy[i]->rect.x = 0;
		enemy[i]->rect.y = 0;
		enemy[i]->pos.x = itemCount * 40;
		enemy[i]->pos.y = 40 * rowCount;
		enemy[i]->startPos = enemy[i]->pos.x;
		enemy[i]->rowPosID = 40 * (11 - itemCount);
		enemy[i]->goLeft = 0;
		enemy[i]->hitbox.width = enemy[i]->rect.width;
		enemy[i]->hitbox.height = enemy[i]->rect.height;
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

		if (enemy[e]->pos.x >= 640 - (enemy[e]->rect.width + enemy[e]->rowPosID) && enemy[e]->goLeft == 0)
		{
			enemy[e]->goLeft = 1;
		}

		if (enemy[e]->pos.x <= enemy[e]->startPos + enemy[e]->rect.width && enemy[e]->goLeft == 1)
		{
			enemy[e]->goLeft = 0;
		}

		enemy[e]->shootTimer += 1 * DeltaTime;

		/*
		c = enemy[1]->shootTimer += 1 * DeltaTime;;
		printf("%0.8f\n", c);
		*/

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
			addEnemyBullet(enemy[e]->pos.x + enemy[e]->rect.width / 2 - 4, enemy[e]->pos.y - 4);
			PlaySound(snd_pusher);
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

			Vector2 pos = { enemy[e]->pos.x, enemy[e]->pos.y };
			DrawTextureRec(enemy[e]->tex, enemy[e]->rect, pos, WHITE);
		}
	}
}

/*
Player
*/

void initPlayer()
{
	player.tex = player_tex;
	p_move = 640 / 2 - player.tex.width / 2;
	player.pos.y = (480 - 60) - player.tex.height / 2;
	player.hitbox.width = player.tex.width;
	player.hitbox.height = player.tex.height;
	player.hitbox.x = player.pos.x;
	player.hitbox.y = player.pos.y;
	player.alive = 1;
}

void input()
{
	//continuous-response keys
	if (IsKeyDown(KEY_RIGHT))
	{
		p_move += 15;
	}
	else if (IsKeyDown(KEY_LEFT))
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
	else if (player.pos.x >= 640 - player.tex.height)
	{
		player.pos.x = 640 - player.tex.width;
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
		bullets[i]->tex = bullet_tex;
		bullets[i]->pos.x = x;
		bullets[i]->pos.y = y;
		bullets[i]->hitbox.width = 6;
		bullets[i]->hitbox.height = 36;
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
		DrawTexture(bullets[i]->tex, bullets[i]->pos.x, bullets[i]->pos.y, WHITE);
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
		explo[i]->tex = explo_tex;
		explo[i]->pos.x = x;
		explo[i]->pos.y = y;
		explo[i]->rect.width = 128;
		explo[i]->rect.height = 129;
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
		Vector2 pos = { explo[i]->pos.x, explo[i]->pos.y };
		DrawTextureRec(explo[i]->tex, explo[i]->rect, pos, WHITE);
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
			if (bullets[i]->pos.x > enemy[e]->pos.x && bullets[i]->pos.x < enemy[e]->pos.x + enemy[e]->hitbox.width &&
				bullets[i]->pos.y > enemy[e]->pos.y && bullets[i]->pos.y < enemy[e]->pos.y + enemy[e]->hitbox.height &&
				enemy[e]->alive == 1)
			{
				enemy[e]->alive = 0;
				addExplo(bullets[i]->pos.x - 128 / 2, bullets[i]->pos.y - 128 / 2);
				removeBullet(i);
				PlaySound(snd_explo);
				score += 100;
				enemies_killed--;
				//printf("BOOM!\n");
				break;
			}
		}
	}

	int b;
	for (b = 0; b < MAX_ENEMY_BULLETS; b++) if (enemy_bullets[b])
	{
		if (enemy_bullets[b]->pos.x > player.pos.x && enemy_bullets[b]->pos.x < player.pos.x + player.hitbox.width &&
			enemy_bullets[b]->pos.y > player.pos.y && enemy_bullets[b]->pos.y < player.pos.y + player.hitbox.height &&
			player.alive == 1)
		{
			player.alive = 0;
			addExplo(player.pos.x - 128 / 2, player.pos.y - 128 / 2);
			removeEnemyBullet(i);
			PlaySound(snd_explo);
			//printf("BOOM!\n");
			break;
		}
	}
}

void restart()
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
	p_move = 640 / 2 - player.tex.width / 2;
	player.alive = 1;
}

int main(int argc, char* argv[]) {

	//Start raylib
	InitWindow(640, 480, "Space Invaders");
	InitAudioDevice(); // Initialize audio device
	SetTargetFPS(60);
	preload_assets();

	//(re)set everything
	score = 0;
	rowCount = 0;
	itemCount = 0;
	currentFrame = 0;
	StopMusicStream(music);

	for (int e = 0; e < MAX_ENEMIES; e++)
	{
		enemy[e] = NULL;
	}

	for (int e = 0; e < MAX_ENEMY_BULLETS; e++)
	{
		enemy_bullets[e] = NULL;
	}

	for (int e = 0; e < MAX_BULLETS; e++)
	{
		bullets[e] = NULL;
	}

	for (int e = 0; e < MAX_EXPLO; e++)
	{
		explo[e] = NULL;
	}

	//begin
	BeginDrawing();
	DrawTexture(fmg_tex, 0, 0, WHITE);
	EndDrawing();
	std::this_thread::sleep_for(std::chrono::milliseconds(2000));
	//UnloadTexture(fmg_splash);

	//Load image
	space = space_tex;

	//init stuff
	initEnemies();
	initPlayer();

	//font
	Color Color = { 255, 255, 255 };
	char textBuffer[64];
	sprintf_s(textBuffer, "SCORE: % 05d", score);
	Vector2 score_txtsize = MeasureTextEx(vermin_ttf, textBuffer, 24, 0);
	Vector2 score_pos = { 640 - score_txtsize.x - 10, 20 };

	Vector2 youWin_pos;
	Vector2 youWin_txtsize = MeasureTextEx(vermin_ttf, "Game Over!", 64, 0);
	youWin_pos.x = 640 / 2 - youWin_txtsize.x / 2;
	youWin_pos.y = 480 / 2 - youWin_txtsize.y / 2;

	Vector2 game_over_pos;
	Vector2 game_over_txtsize = MeasureTextEx(vermin_ttf, "Game Over!", 64, 0);
	game_over_pos.x = 640 / 2 - game_over_txtsize.x / 2;
	game_over_pos.y = 480 / 2 - game_over_txtsize.y / 2;

	//Play the music
	PlayMusicStream(music);
	SetMusicLoopCount(music, -1);

	while (!WindowShouldClose())
	{
		UpdateMusicStream(music);        // Update music buffer with new stream data
		DeltaTime = (clock() / CLOCKS_PER_SEC) - lastTick;

		//Handle events on queue
		if (IsKeyPressed(KEY_SPACE) && player.alive == 1)
		{
			addBullet(player.pos.x + player.tex.width / 2 - 3, player.pos.y);
			PlaySound(snd_blaster);
		}

		if (IsKeyPressed(KEY_ENTER))
		{
			restart();
		}

		BeginDrawing();

		ClearBackground(RAYWHITE);

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

		DrawTexture(space, 0, 0, WHITE);

		drawExplo();
		drawEnemies();
		drawBullet();
		drawEnemyBullet();

		if (player.alive == 1) {
			DrawTexture(player.tex, player.pos.x, player.pos.y, WHITE);
		}
		else
		{
			//DrawTextEx(vermin_ttf, "Game Over!", game_over_pos, 64, 0, WHITE);
			DrawTexture(gameover_tex, 0, 0, WHITE);
		}

		if (enemies_killed <= 0)
		{
			//DrawTextEx(vermin_ttf, "You Win!", youWin_pos, 64, 0, WHITE);
			DrawTexture(win_tex, 0, 0, WHITE);
		}

		//this ugly block is updating the score
		sprintf_s(textBuffer, "SCORE: % 05d", score);
		DrawTextEx(vermin_ttf, textBuffer, score_pos, 24, 0, WHITE);

		lastTick = (clock() / CLOCKS_PER_SEC);
		std::this_thread::sleep_for(std::chrono::milliseconds(30));

		EndDrawing();
	}

	UnloadTexture(player.tex);
	UnloadTexture(space);

	//Quit
	CloseWindow();

	return 0;
}

