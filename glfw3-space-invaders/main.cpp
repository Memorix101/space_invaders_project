#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <FMOD/fmod.h>
#include "glfx.h"
#include "font.h"
#include <cstring>
#include <cstdio>
#include <cstdlib>

struct player_t {
	struct rect hitbox;
	struct vector2 pos;
	int speed;
	GLuint tex;
	int alive; //bool
};

struct bullet_t
{
	struct rect hitbox;
	struct vector2 pos;
	int speed;
	GLuint tex;
};

struct enemy_bullet_t
{
	struct rect hitbox;
	struct vector2 pos;
	int speed;
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
	int speed;
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

//enemy
#define MAX_ENEMY_BULLETS 100
#define MAX_ENEMIES 40
struct enemy_t* enemy[MAX_ENEMIES] = { NULL };
int enemies_killed = MAX_ENEMIES;

//stuff
struct bullet_t* bullets[MAX_BULLETS] = { NULL };
struct enemy_bullet_t* enemy_bullets[MAX_ENEMY_BULLETS] = { NULL };
#define MAX_EXPLO 100
struct explo_t* explo[MAX_EXPLO] = { NULL };

int rowCount = 0;
int itemCount = 0;
int lastTick;
int gameover = 0;
int score = 0;
float deltaTime = 0.0f;
float lastdelta = 0.0f;
float curTime = 0.0f;
int start_game = 0; //bool
float gameSessionTime = 0.f; // time since start
int gpad_firePressed = 0; // bool
int key_firePressed = 0; // bool

//FMOD
FMOD_SYSTEM* fmod_system;
FMOD_SOUND* snd_blaster, * snd_pusher, * snd_explo, * music;
FMOD_CHANNEL* channel = 0;
FMOD_RESULT result;

//GLFW
GLFWwindow* window;
const GLFWvidmode* mode;
struct GLFWgamepadstate state;
int joystickIsConnected = -1;

//sprites and stuff
GLuint space3_tex2d;
GLuint enemy_tex2d;
GLuint player_tex2d;
GLuint bullet_tex2d;
GLuint enemy_bullet_tex2d;
GLuint explo_tex2d;
GLuint fmg_splash_tex2d;
GLuint gameover_tex2D;
GLuint win_tex2d;

void preload_assets()
{
	space3_tex2d = load_texture("rd/space3.png");
	enemy_tex2d = load_texture("rd/invader32x32x4.png");
	player_tex2d = load_texture("rd/player.png");
	bullet_tex2d = load_texture("rd/bullet.png");
	enemy_bullet_tex2d = load_texture("rd/enemy-bullet.png");
	explo_tex2d = load_texture("rd/explode.png");
	fmg_splash_tex2d = load_texture("rd/fmg_splash.psd");;
	gameover_tex2D = load_texture("rd/gameover_ui.png");
	win_tex2d = load_texture("rd/win_ui.png");
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
		enemy_bullets[i]->tex = enemy_bullet_tex2d;
		enemy_bullets[i]->pos = { (int)x, (int)y };
		enemy_bullets[i]->speed = 450;
		enemy_bullets[i]->hitbox = { 0, 0,9, 9 };
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
		int i = found;
		enemy[i] = (enemy_t*)malloc(sizeof(enemy)); //visual studio needs that "stupid" cast to operate >_>
		enemy[i]->tex = enemy_tex2d;
		enemy[i]->alive = 1;
		enemy[i]->rect = { 0, 0, 32, 32 };
		enemy[i]->pos = { itemCount * 40, 40 * rowCount };
		enemy[i]->speed = 150;
		enemy[i]->startPos = enemy[i]->pos.x;
		enemy[i]->rowPosID = 40 * (11 - itemCount);
		enemy[i]->goLeft = 0;
		enemy[i]->hitbox = { 0,0, 32, 32 };
		//enemy[i]->hitbox = {enemy[i]->pos.x,enemy[i]->pos.y};
		enemy[i]->shoot = 0;
		enemy[i]->shootTimer = 0;
		enemy[i]->currentFrame = 0.0f;
		enemy[i]->shootTimeLimit = (rand() % (20 - 3)) + 3; // MAX - MIN + MIN
	}
}

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
		addEnemy();
	}
}

void animatorEnemies()
{
	for (int e = 0; e < MAX_ENEMIES; e++) if (enemy[e]) {
		enemy[e]->currentFrame += 10 * deltaTime;
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

	for (int e = 0; e < MAX_ENEMIES; e++) if (enemy[e]) {
		if (enemy[e]->goLeft == 0)
		{
			enemy[e]->pos.x += enemy[e]->speed * deltaTime;
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
			FMOD_System_PlaySound(fmod_system, snd_pusher, 0, false, &channel);
		}

		enemy[e]->hitbox.x = enemy[e]->pos.x;
		enemy[e]->hitbox.y = enemy[e]->pos.y;
	}
}

void drawEnemies()
{
	for (int e = 0; e < MAX_ENEMIES; e++) if (enemy[e]) {
		if (enemy[e]->alive == 1) {
			draw(enemy[e]->tex, enemy[e]->pos, enemy[e]->hitbox, enemy[e]->rect, 128, 32);
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
		bullets[i]->tex = bullet_tex2d;
		bullets[i]->pos = { (int)x, (int)y };
		bullets[i]->speed = 550;
		bullets[i]->hitbox = { 0, 0, 6, 36 };
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
		draw(bullets[i]->tex, bullets[i]->pos, bullets[i]->hitbox);
	}
}


//Player
void initPlayer()
{
	player.tex = player_tex2d;
	player.hitbox = { 0,0,28, 21 };
	player.pos = { 640 / 2 - player.hitbox.w / 2, (480 - 60) - player.hitbox.h / 2 };
	player.speed = 350;
	player.alive = 1;
}

void fire()
{
	addBullet(player.pos.x + player.hitbox.w / 2 - 3, player.pos.y - player.hitbox.w / 2);
	FMOD_System_PlaySound(fmod_system, snd_blaster, 0, false, &channel);
}

void input(GLFWwindow * w)
{
	//continuous-response keys
	if (glfwGetKey(w, GLFW_KEY_RIGHT) == GLFW_PRESS || glfwGetKey(w, GLFW_KEY_D) == GLFW_PRESS)
	{
		player.pos.x += player.speed * deltaTime;
	}

	if (glfwGetKey(w, GLFW_KEY_LEFT) == GLFW_PRESS || glfwGetKey(w, GLFW_KEY_A) == GLFW_PRESS)
	{
		player.pos.x -= player.speed * deltaTime;
	}

	if (glfwGetKey(w, GLFW_KEY_SPACE) == GLFW_PRESS && key_firePressed == 0)
	{
		key_firePressed = 1;
		fire();
	}
	else if (glfwGetKey(w, GLFW_KEY_SPACE) == GLFW_RELEASE && key_firePressed == 1)
	{
		key_firePressed = 0;
	}

	if (joystickIsConnected == 1)
	{
		if (glfwGetGamepadState(GLFW_JOYSTICK_1, &state))
		{
			if (state.buttons[GLFW_GAMEPAD_BUTTON_A])
			{
				if (gpad_firePressed == 0)
				{
					gpad_firePressed = 1;
					fire();
				}
			}
			else {
				gpad_firePressed = 0;
			}

			if (state.axes[GLFW_GAMEPAD_AXIS_LEFT_X] >= 0.5f || (state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_RIGHT]))
			{
				player.pos.x += player.speed * deltaTime;
			}

			if (state.axes[GLFW_GAMEPAD_AXIS_LEFT_X] <= -0.5f || (state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_LEFT]))
			{
				player.pos.x -= player.speed * deltaTime;
			}
		}
	}
}

void updatePlayer(GLFWwindow * w)
{
	input(w);

	if (player.pos.x <= 0)
	{
		player.pos.x = 0;
	}
	else if (player.pos.x >= 640 - player.hitbox.w)
	{
		player.pos.x = 640 - player.hitbox.w;
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
		explo[i]->tex = explo_tex2d;
		explo[i]->pos = { (int)x, (int)y };
		explo[i]->rect = { 0, 0,128, 128 };
		explo[i]->animationFinished = 0;
		explo[i]->currentFrame_ex = 0;
	}
}

void animatorExplo()
{
	for (int e = 0; e < MAX_EXPLO; e++) if (explo[e]) {
		explo[e]->currentFrame_ex += 25 * deltaTime;
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

//Something something
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
				FMOD_System_PlaySound(fmod_system, snd_explo, 0, false, &channel);
				score += 100;
				enemies_killed--;
				break;
			}
		}
	}

	for (int e = 0; e < MAX_ENEMIES; e++) if (enemy[e])
	{
		if (enemy[e]->alive == 0)
		{
			removeEnemy(e);
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
			FMOD_System_PlaySound(fmod_system, snd_explo, 0, false, &channel);
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
	memset(bullets, 0, sizeof(bullets));
	memset(enemy_bullets, 0, sizeof(enemy_bullets));
	memset(explo, 0, sizeof(explo));
	rowCount = 0;
	itemCount = 0;
	initEnemies();
	player.pos.x = 640 / 2 - player.hitbox.w / 2;
	player.alive = 1;
}

int main(int argc, char* argv[]) {

	// Init GLFW
	glfwInit();
	glfwDefaultWindowHints();
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	window = glfwCreateWindow(640, 480, "GLFW3 Space Invaders", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	//set window icon
	GLFWimage images[1]; 
	images[0] = load_texture_to_GLFWimage("rd/icon.png"); //rgba channels 
	glfwSetWindowIcon(window, 1, images); 
	texture_free(images[0].pixels);

	// Get the resolution of the primary monitor
	mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

	// Center our window
	glfwSetWindowPos(window, (mode->width - 640) / 2, (mode->height - 480) / 2);

	// Make the window's context current 
	glfwMakeContextCurrent(window);

	// Initialize GLEW to setup the OpenGL Function pointers
	glewExperimental = GL_TRUE;
	glewInit();

	// Init OpenGL
	initGL(640, 480);

	// Setup TTF stuff
	init_ttf("rd/vermin_vibes_1989.ttf", 24);
	char textBuffer[64];
	sprintf(textBuffer, "SCORE: % 05d", score);

	// load sprites
	preload_assets();

	// Init FMOD Soundsystem
	FMOD_System_Create(&fmod_system);
	FMOD_System_Init(fmod_system, 32, FMOD_INIT_NORMAL, NULL);

	// Setup controller
	joystickIsConnected = glfwJoystickPresent(GLFW_JOYSTICK_1);

	if (joystickIsConnected == 1)
	{
		if (glfwJoystickIsGamepad(GLFW_JOYSTICK_1))
		{
			printf("Conntected controller: %s\n", glfwGetGamepadName(GLFW_JOYSTICK_1));
		}
		else
		{
			joystickIsConnected = -1;
		}
	}

	// Hello to the main cast
	initEnemies();
	initPlayer();

	// Load sounds
	FMOD_System_CreateSound(fmod_system, "rd/blaster.ogg", FMOD_CREATESAMPLE, 0, &snd_blaster);
	FMOD_System_CreateSound(fmod_system, "rd/explode1.ogg", FMOD_CREATESAMPLE, 0, &snd_explo);
	FMOD_System_CreateSound(fmod_system, "rd/pusher.ogg", FMOD_CREATESAMPLE, 0, &snd_pusher);
	FMOD_System_CreateSound(fmod_system, "rd/bodenstaendig.ogg", FMOD_LOOP_NORMAL, 0, &music);

	// Play music
	FMOD_System_PlaySound(fmod_system, music, 0, false, &channel);

	// Loop until the user closes the window
	while (!glfwWindowShouldClose(window))
	{
		// main input callback
		if (glfwGetKey(window, GLFW_KEY_ESCAPE))
		{
			glfwSetWindowShouldClose(window, GLFW_TRUE);
		}

		if (glfwGetKey(window, GLFW_KEY_ENTER) && gameover == 1)
		{
			reset();
		}

		// controller input
		if (joystickIsConnected == 1)
		{
			if (glfwGetGamepadState(GLFW_JOYSTICK_1, &state))
			{
				if (state.buttons[GLFW_GAMEPAD_BUTTON_START] && gameover == 1)
				{
					reset();
				}

				if (state.buttons[GLFW_GAMEPAD_BUTTON_BACK])
				{
					glfwSetWindowShouldClose(window, GLFW_TRUE);
				}
			}
		}

		// game loop
		curTime = (float)glfwGetTime();
		deltaTime = curTime - lastdelta;
		lastdelta = curTime;

		gameSessionTime += deltaTime;

		float ratio;
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		ratio = width / (float)height;

		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (gameSessionTime >= 5.0f)
		{
			start_game = 1;
		}

		if (start_game == 0)
		{
			draw(fmg_splash_tex2d, { 0, 0 });
		}

		if (start_game == 1)
		{
			updateExplo();
			updateBullet();
			updateEnemyBullet();
			updateEnemies();

			if (player.alive == 1)
				updatePlayer(window);

			updateLogic();

			draw(space3_tex2d, { 0, 0 });
			drawExplo();
			drawEnemies();
			drawBullet();
			drawEnemyBullet();

			if (player.alive == 1) {
				draw(player.tex, player.pos, player.hitbox);
			}
			else
			{
				draw(gameover_tex2D, { 0, 0 });
				gameover = 1;
			}

			if (enemies_killed <= 0)
			{
				draw(win_tex2d, { 0, 0 });
				gameover = 1;
			}

			sprintf(textBuffer, "SCORE: % 05d", score);
			ttf_print(30 + 420, 30, textBuffer);
		}

		// Swap front and back buffers
		glfwSwapBuffers(window);

		// Poll for and process events
		glfwPollEvents();
	}

	FMOD_System_Close(fmod_system);
	glfwTerminate();
	return 0;
}
