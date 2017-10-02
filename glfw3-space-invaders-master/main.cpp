// DC_SDL_Game.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <GLFW/glfw3.h>
#include <SOIL.h>
#include <cstdlib>
#include "glfx.h"

float Timestep = 0;
float lastdelta = 0;
float curTime = 0;

struct player_t {
	struct rect hitbox;
	struct vector2 pos;
	GLuint tex;
	int alive; //bool
};

struct bullet_t
{
	struct rect hitbox;
	struct vector2 pos;
	GLuint tex;
};

struct enemy_bullet_t
{
	struct rect hitbox;
	struct vector2 pos;
	GLuint tex;
};

struct explo_t
{
	struct vector2 pos;
	struct rect rect;
	struct rect size;
	GLuint tex;
	int animationFinished; //bool
	float currentFrame_ex;
};

struct enemy_t {
	struct rect hitbox;
	struct vector2 pos;
	struct rect rect;
	GLuint tex;
	int goLeft; //bool
	int startPos;
	int rowPosID;
	int alive; //bool
	float shootTimer;
	int shoot; //bool
	float shootTimeLimit;
	float currentFrame;
};

//player
#define MAX_BULLETS 50
struct player_t player;
int p_move;

//enemy
#define MAX_ENEMY_BULLETS 100
#define MAX_ENEMIES 40
struct enemy_t *enemy[MAX_ENEMIES] = { NULL };

//stuff
struct bullet_t *bullets[MAX_BULLETS] = { NULL };
struct enemy_bullet_t * enemy_bullets[MAX_ENEMY_BULLETS] = { NULL };
#define MAX_EXPLO 100
struct explo_t *explo[MAX_EXPLO] = { NULL };
int rowCount = 0;
int itemCount = 0;
int lastTick;

int start_game = 0; //bool
float gameSessionTime = 0.f; // time since start
int button_down = 0; // bool

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
		enemy_bullets[i] = (enemy_bullet_t *)malloc(sizeof(enemy_bullet_t));

		enemy_bullets[i]->tex = SOIL_load_OGL_texture
		(
			"rd/enemy-bullet.png",
			SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID,
			SOIL_FLAG_MIPMAPS | SOIL_FLAG_COMPRESS_TO_DXT
		);

		enemy_bullets[i]->pos.x = x;
		enemy_bullets[i]->pos.y = y;
		enemy_bullets[i]->hitbox.w = 9;
		enemy_bullets[i]->hitbox.h = 9;
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
		enemy_bullets[i]->pos.y += 350 * Timestep;

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
		draw(enemy_bullets[i]->tex, enemy_bullets[i]->pos, enemy_bullets[i]->hitbox);
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
		enemy[i] = (enemy_t *)malloc(sizeof(enemy)); //visual studio needs that "stupid" cast to operate >_>
		enemy[i]->tex = SOIL_load_OGL_texture
		(
			"rd/invader32x32x4.png",
			SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID,
			SOIL_FLAG_MIPMAPS | SOIL_FLAG_COMPRESS_TO_DXT
		);
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
		enemy[i]->hitbox.w = 32;
		enemy[i]->hitbox.h = 32;
		enemy[i]->hitbox.x = enemy[i]->pos.x;
		enemy[i]->hitbox.y = enemy[i]->pos.y;
		enemy[i]->shoot = 0;
		enemy[i]->shootTimer = 0;
		enemy[i]->currentFrame = 0;
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
	int e;
	for (e = 0; e < MAX_ENEMIES; e++) if (enemy[e]) {
		enemy[e]->currentFrame += 10 * Timestep;
		enemy[e]->rect.x = (int)enemy[e]->currentFrame * 32;

		if (enemy[e]->currentFrame >= 3)
		{
			enemy[e]->currentFrame = 0;
		}
	}
}

void updateEnemies()
{
	animatorEnemies();

	const int moveSpeed = 150;

	int e;
	for (e = 0; e < MAX_ENEMIES; e++) if (enemy[e]) {
		if (enemy[e]->goLeft == 0)
		{
			enemy[e]->pos.x += moveSpeed * Timestep;
		}

		if (enemy[e]->goLeft == 1)
		{
			enemy[e]->pos.x -= moveSpeed * Timestep;
		}

		if (enemy[e]->pos.x >= 640 - (enemy[e]->rect.w + enemy[e]->rowPosID) && enemy[e]->goLeft == 0)
		{
			enemy[e]->goLeft = 1;
		}

		if (enemy[e]->pos.x <= enemy[e]->startPos + enemy[e]->rect.w && enemy[e]->goLeft == 1)
		{
			enemy[e]->goLeft = 0;
		}

		enemy[e]->shootTimer += 1 * Timestep;

		//c = enemy[1]->shootTimer += 1 * Timestep;;
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
			draw(enemy[e]->tex, enemy[e]->pos, enemy[e]->hitbox, enemy[e]->rect, 128, 32);
		}
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
		bullets[i] = (bullet_t *)malloc(sizeof(bullet_t));
		bullets[i]->tex = SOIL_load_OGL_texture
		(
			"rd/bullet.png",
			SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID,
			SOIL_FLAG_MIPMAPS | SOIL_FLAG_COMPRESS_TO_DXT
		);
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
		bullets[i]->pos.y -= 750 * Timestep;

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
		draw(bullets[i]->tex, bullets[i]->pos, bullets[i]->hitbox);
	}
}

/*
   Player
*/

void initPlayer()
{
	player.tex = SOIL_load_OGL_texture
	(
		"rd/player.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_COMPRESS_TO_DXT
	);
	player.hitbox.w = 28;
	player.hitbox.h = 21;
	p_move = 640 / 2 - player.hitbox.w / 2;
	player.pos.y = (480 - 60) - player.hitbox.h / 2;
	player.hitbox.x = player.pos.x;
	player.hitbox.y = player.pos.y;
	player.alive = 1;
}

void input(GLFWwindow* w)
{
	//continuous-response keys
	if (glfwGetKey(w, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		p_move += 350 * Timestep;
	}
	if (glfwGetKey(w, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		p_move -= 350 * Timestep;
	}

	if (glfwGetKey(w, GLFW_KEY_SPACE) == GLFW_PRESS && button_down == 0)
	{
		// Move up		
		button_down = 1;
		//printf("BUTTON\n");
		addBullet(player.pos.x + player.hitbox.w / 2 - 3, player.pos.y);
	}
	else if (glfwGetKey(w, GLFW_KEY_SPACE) == GLFW_RELEASE && button_down == 1)
	{
		button_down = 0;
		//printf("RELEASED\n");
	}
}

void updatePlayer(GLFWwindow* w)
{
	input(w);

	player.pos.x = p_move;

	player.hitbox.x = player.pos.x;
	player.hitbox.y = player.pos.y;

	if (player.pos.x <= 0)
	{
		player.pos.x = 0;
	}
	else if (player.pos.x >= 640 - player.hitbox.w)
	{
		player.pos.x = 640 - player.hitbox.w;
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
		explo[i] = (explo_t *)malloc(sizeof(explo_t));
		explo[i]->tex = SOIL_load_OGL_texture
		(
			"rd/explode.png",
			//"nalacat_512.jpg",
			SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID,
			SOIL_FLAG_MIPMAPS | SOIL_FLAG_COMPRESS_TO_DXT
		);
		explo[i]->pos.x = x;
		explo[i]->pos.y = y;
		explo[i]->rect.w = 128;
		explo[i]->rect.h = 128;
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
		explo[e]->currentFrame_ex += 25 * Timestep;
		explo[e]->rect.x = (int)explo[e]->currentFrame_ex * 128;

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
		draw(explo[i]->tex, explo[i]->pos, explo[i]->rect, explo[i]->rect, 2048, 128);
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
			//printf("BOOM!\n");
			break;
		}
	}

}

int main(int argc, char *argv[]) {

	//Setup GLFW 
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	glfwDefaultWindowHints();
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	window = glfwCreateWindow(640, 480, "GLFW3 Space Invaders", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	// Get the resolution of the primary monitor
	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

	// Center our window
	glfwSetWindowPos(window, (mode->width - 640) / 2,(mode->height - 480) / 2);

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	initGL(640, 480);


	GLuint fmg_splash = SOIL_load_OGL_texture
	(
		"rd/fmg_splash.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_COMPRESS_TO_DXT
	);

	vector2 vz;
	vz.x = 0;
	vz.y = 0;

	rect std_res;
	std_res.w = 640;
	std_res.h = 480;

	//Load image
	GLuint space = SOIL_load_OGL_texture
	(
		"rd/space3.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_COMPRESS_TO_DXT
	);

	rect r1;
	r1.w = 640;
	r1.h = 480;

	initEnemies();
	initPlayer();

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		curTime = (float)glfwGetTime();
		Timestep = curTime - lastdelta;
		lastdelta = curTime;

		gameSessionTime += 1 * Timestep;

		if (gameSessionTime >= 3.f)
		{
			start_game = 1;
		}

		if (start_game == 0) {
			draw(fmg_splash, vz, std_res);
		}

		if (start_game == 1)
		{
			updateExplo();
			updateBullet();
			updateEnemyBullet();
			updateEnemies();

			if (player.alive == 1)
				updatePlayer(window); //player

			updateLogic();

			draw(space, vz, r1);
			drawExplo();
			drawEnemies();
			drawBullet();
			drawEnemyBullet();

			if (player.alive == 1) {
				draw(player.tex, player.pos, player.hitbox);
			}

		}

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}
