// irrlicht_space_invaders.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <cstdlib>
#include <thread>
#include <time.h>
#include <stdio.h>
#include <string>
#include <Irrlicht/irrlicht.h>
#ifdef _WIN32
#include <codecvt>
#endif // _WIN32


using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

#ifdef _IRR_WINDOWS_
#pragma comment(lib, "Irrlicht.lib")
#pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")
#endif

struct player_t {
	vector2d<s32> size;
	position2d<s32> pos;
	video::ITexture* tex;
	int alive; //bool
};

struct bullet_t
{
	vector2d<s32> size;
	position2d<s32> pos;
	video::ITexture* tex;
};

struct enemy_bullet_t
{
	vector2d<s32> size;
	position2d<s32> pos;
	video::ITexture* tex;
};

struct explo_t
{
	vector2d<s32> size;
	position2d<s32> pos;
	rect<s32> animRect;
	video::ITexture* tex;
	int animationFinished; //bool
	int currentFrame_ex;
};

struct enemy_t {
	position2d<s32> pos;
	vector2d<s32> size;
	rect<s32> animRect;
	video::ITexture* tex;
	int goLeft; //bool
	int startPos;
	int rowPosID;
	int alive; //bool
	float shootTimer;
	int shoot; //bool
	float shootTimeLimit;
};

//base stuff
video::ITexture* space;
video::ITexture* screen;
int screen_width = 640;
int screen_height = 480;

//player
#define MAX_BULLETS 50
struct player_t player;
int p_move;

//enemy
#define MAX_ENEMY_BULLETS 100
#define MAX_ENEMIES 40
struct enemy_t* enemy[MAX_ENEMIES] = { NULL };
float currentFrame = 0.0f;
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
/*
Music music;
Sound snd_pusher;
Sound snd_blaster;
Sound snd_explo;
*/

video::ITexture* enemy_tex;
video::ITexture* player_tex;
video::ITexture* bullet_tex;
video::ITexture* enemy_bullet_tex;
video::ITexture* explo_tex;
video::ITexture* fmg_tex;
video::ITexture* space_tex;
video::ITexture* gameover_tex;
video::ITexture* win_tex;

//SpriteFont vermin_ttf;

// input 
class MyEventReceiver : public IEventReceiver
{
public:
	// This is the one method that we have to implement
	virtual bool OnEvent(const SEvent& event)
	{

		// Remember whether each key is down or up
		if (event.EventType == irr::EET_KEY_INPUT_EVENT) {
			KeyIsDown[event.KeyInput.Key] = event.KeyInput.PressedDown;
		}

		return false;
	}

	// This is used to check whether a key is being held down
	virtual bool KeyDown(EKEY_CODE keyCode) const
	{
		return KeyIsDown[keyCode];
	}


	MyEventReceiver()
	{
		for (u32 i = 0; i < KEY_KEY_CODES_COUNT; ++i) {
			KeyIsDown[i] = false;
		}
	}

private:
	// We use this array to store the current state of each key
	bool KeyIsDown[KEY_KEY_CODES_COUNT];
};

// engine 
IVideoDriver* driver = nullptr;
ISceneManager* smgr = nullptr;
IGUIEnvironment* guienv = nullptr;
IrrlichtDevice* device = nullptr;
MyEventReceiver receiver;

void drawSprite(IVideoDriver* driver, ITexture* img, position2d<s32> pos, rect<s32> destRect, SColor color) {

	driver->draw2DImage(img, pos,destRect, 0, color, true);
}

#ifdef _WIN32
// Convert char string to wchar_t string
std::wstring convertWString(const char* input) {
	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	return  converter.from_bytes(input);
}

#endif // _WIN32

void preload_assets()
{
	//load tex
	fmg_tex = driver->getTexture("rd/fmg_splash.png");
	space_tex = driver->getTexture("rd/space3.png");
	enemy_bullet_tex = driver->getTexture("rd/enemy-bullet.png");
	enemy_tex = driver->getTexture("rd/invader32x32x4.png");
	bullet_tex = driver->getTexture("rd/bullet.png");
	player_tex = driver->getTexture("rd/player.png");
	explo_tex = driver->getTexture("rd/explode.png");
	gameover_tex = driver->getTexture("rd/gameover_ui.png");
	win_tex = driver->getTexture("rd/win_ui.png");

	//load audio
	/*
	music = LoadMusicStream("rd/bodenstaendig.ogg");
	snd_blaster = LoadSound("rd/blaster.ogg");
	snd_explo = LoadSound("rd/explode1.wav");
	snd_pusher = LoadSound("rd/pusher.wav");
	*/

	//load font
	guienv->getSkin()->setFont(guienv->getFont("./rd/vermin_vibes_1989.xml"));
}

int setup_engine() {
	device = createDevice(video::EDT_OPENGL, dimension2d<u32>(screen_width, screen_height), 16,
			false, false, false, &receiver);

	if (!device)
		return 1;

	device->setWindowCaption(L"Irrlicht Engine Space Invaders");
	driver = device->getVideoDriver();
	smgr = device->getSceneManager();
	guienv = device->getGUIEnvironment();
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
		enemy_bullets[i]->pos = position2d<s32>(x, y);
		enemy_bullets[i]->size = vector2d<s32>(enemy_bullets[i]->tex->getSize().Width, enemy_bullets[i]->tex->getSize().Height);
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
		enemy_bullets[i]->pos.Y += 15;

		if (enemy_bullets[i]->pos.Y >= screen_height - 9)
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
		//driver->draw2DImage(enemy_bullets[i]->tex, position2d<s32>(enemy_bullets[i]->pos.X, enemy_bullets[i]->pos.Y));
		drawSprite(driver, enemy_bullets[i]->tex, position2d<s32>(enemy_bullets[i]->pos.X, enemy_bullets[i]->pos.Y), 
			rect<s32>(0, 0, enemy_bullets[i]->size.X, enemy_bullets[i]->size.Y), video::SColor(255, 255, 255, 255));
	}
}

/*
Enemies
*/

void addEnemy()
{
	for (int i = 0; i < MAX_ENEMIES; i++)
	{
		if (i % 10 == 0)
		{
			itemCount = 0;
			rowCount++;
		}
		itemCount++;

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
			enemy[i]->tex = enemy_tex; //invader32x32x4
			enemy[i]->alive = 1;
			enemy[i]->rowPosID = 40 * (11 - itemCount);
			enemy[i]->goLeft = 0;
			enemy[i]->pos = position2d<s32>(itemCount * 40, 40 * rowCount);
			enemy[i]->size = vector2d<s32>(32, 32);
			enemy[i]->animRect = rect<s32>(0, 0, 32, 32);
			enemy[i]->startPos = enemy[i]->pos.X;
			enemy[i]->shoot = 0;
			enemy[i]->shootTimer = 0;
			enemy[i]->shootTimeLimit = (rand() % (20 - 3)) + 3; // MAX - MIN + MIN
		}
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
	currentFrame += 10.0f * DeltaTime;

	if (currentFrame >= 4)
	{
		currentFrame = 0;
	}

	int e;
	for (e = 0; e < MAX_ENEMIES; e++) if (enemy[e]) {
		enemy[e]->animRect = rect<s32>((int)currentFrame * 32, 0, ((int)currentFrame + 1) * 32, 32);
	}

}

void updateEnemies()
{
	animatorEnemies();

	const int moveSpeed = 5;

	int e;
	for (e = 0; e < MAX_ENEMIES; e++) if (enemy[e]) {
		if (enemy[e]->goLeft == 0)
		{
			enemy[e]->pos.X += moveSpeed;
		}

		if (enemy[e]->goLeft == 1)
		{
			enemy[e]->pos.X -= moveSpeed;
		}

		if (enemy[e]->pos.X >= screen_width - (enemy[e]->size.X + enemy[e]->rowPosID) && enemy[e]->goLeft == 0)
		{
			enemy[e]->goLeft = 1;
		}

		if (enemy[e]->pos.X <= enemy[e]->startPos + enemy[e]->size.X && enemy[e]->goLeft == 1)
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
			addEnemyBullet(enemy[e]->pos.X + enemy[e]->size.X / 2 - 4, enemy[e]->pos.Y - 4);
			//PlaySound(snd_pusher);
		}
	}
}

void drawEnemies()
{
	int e;
	for (e = 0; e < MAX_ENEMIES; e++) if (enemy[e]) {
		if (enemy[e]->alive == 1) {
			/*driver->draw2DImage(enemy[e]->tex, position2d<s32>(enemy[e]->pos.X, enemy[e]->pos.Y),
				enemy[e]->animRect, 0, video::SColor(255, 255, 255, 255), true);*/

			drawSprite(driver, enemy[e]->tex, position2d<s32>(enemy[e]->pos.X, enemy[e]->pos.Y), enemy[e]->animRect,video::SColor(255, 255, 255, 255));
		}
	}
}

/*
Player
*/

void initPlayer()
{
	player.tex = player_tex;
	p_move = screen_width / 2 - player.tex->getSize().Width / 2;
	player.pos = position2d<s32>(p_move, (screen_height - 60) - player.tex->getSize().Height / 2);
	player.size = vector2d<s32>(player.tex->getSize().Width, player.tex->getSize().Height);
	player.alive = 1;
}

void input()
{
	//continuous-response keys
	if (receiver.KeyDown(irr::KEY_RIGHT))
	{
		p_move += 15;
	}
	else if (receiver.KeyDown(irr::KEY_LEFT))
	{
		p_move -= 15;
	}
}

void updatePlayer()
{
	input();

	player.pos.X = p_move;

	if (player.pos.X <= 0)
	{
		player.pos.X = 0;
	}
	else if (player.pos.X >= screen_width - player.tex->getSize().Height)
	{
		player.pos.X = screen_width - player.tex->getSize().Width;
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
		bullets[i]->pos.X = x;
		bullets[i]->pos.Y = y;
		bullets[i]->pos = position2d<s32>(x, y);
		bullets[i]->size = vector2d<s32>(6,36);
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
		bullets[i]->pos.Y -= 15;

		if (bullets[i]->pos.Y <= 0)
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
		//driver->draw2DImage(bullets[i]->tex, position2d<s32>(bullets[i]->pos.X, bullets[i]->pos.Y));
		drawSprite(driver, bullets[i]->tex, position2d<s32>(bullets[i]->pos.X, bullets[i]->pos.Y), rect<s32>(0, 0, bullets[i]->size.X, bullets[i]->size.Y), video::SColor(255, 255, 255, 255));
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
		explo[i]->pos = position2d<s32>(x, y);
		explo[i]->size = vector2d<s32>(128, 129);
		explo[i]->animationFinished = 0;
		explo[i]->currentFrame_ex = 0;
	}
}

void animatorExplo()
{
	int e;
	for (e = 0; e < MAX_EXPLO; e++) if (explo[e]) {
		explo[e]->currentFrame_ex += 1;
		explo[e]->animRect = rect<s32>(explo[e]->currentFrame_ex * 128, 0, (explo[e]->currentFrame_ex + 1) * 128, 128);

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
		driver->draw2DImage(explo[i]->tex, position2d<s32>(explo[i]->pos.X, explo[i]->pos.Y),
			explo[i]->animRect, 0, video::SColor(255, 255, 255, 255), true); // rect<s32>(0, 0, 128, 128)

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
			if (bullets[i]->pos.X > enemy[e]->pos.X && bullets[i]->pos.X < enemy[e]->pos.X + enemy[e]->size.X &&
				bullets[i]->pos.Y > enemy[e]->pos.Y && bullets[i]->pos.Y < enemy[e]->pos.Y + enemy[e]->size.Y &&
				enemy[e]->alive == 1)
			{
				enemy[e]->alive = 0;
				addExplo(bullets[i]->pos.X - 128 / 2, bullets[i]->pos.Y - 128 / 2);
				removeBullet(i);
				//PlaySound(snd_explo);
				score += 100;
				enemies_killed--;
				//printf("BOOM!\n");
				break;
			}

			/*if (bullets[i]->pos.isRectCollided(enemy[e]->pos)) {
				enemy[e]->alive = 0;
				addExplo(bullets[i]->pos.X - 128 / 2, bullets[i]->pos.Y - 128 / 2);
				removeBullet(i);
				//PlaySound(snd_explo);
				score += 100;
				enemies_killed--;
				//printf("BOOM!\n");
				break;
			}*/
		}
	}


	int b;
	for (b = 0; b < MAX_ENEMY_BULLETS; b++) if (enemy_bullets[b])
	{
		if (enemy_bullets[b]->pos.X > player.pos.X && enemy_bullets[b]->pos.X < player.pos.X + player.size.X &&
			enemy_bullets[b]->pos.Y > player.pos.Y && enemy_bullets[b]->pos.Y < player.pos.Y + player.size.Y &&
			player.alive == 1)
		{
			player.alive = 0;
			addExplo(player.pos.X - 128 / 2, player.pos.Y - 128 / 2);
			removeEnemyBullet(i);
			//PlaySound(snd_explo);
			//printf("BOOM!\n");
			break;
		}

		/*if (enemy_bullets[b]->pos.isRectCollided(player.pos)) {
			player.alive = 0;
			addExplo(player.pos.X - 128 / 2, player.pos.Y - 128 / 2);
			removeEnemyBullet(i);
			//PlaySound(snd_explo);
			//printf("BOOM!\n");
			break;
		}*/
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
	p_move = screen_width / 2 - player.tex->getSize().Width / 2;
	player.alive = 1;
}

int main()
{
	//Start Irrlicht
	if (setup_engine() == 1) {
		perror("Could not open device!");
	}

	preload_assets();

	//(re)set everything
	score = 0;
	rowCount = 0;
	itemCount = 0;
	currentFrame = 0;
	//StopMusicStream(music);

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
	driver->beginScene(true, true, video::SColor(255, 120, 102, 136));
	driver->draw2DImage(fmg_tex, position2d<s32>(0, 0));
	driver->endScene();
	std::this_thread::sleep_for(std::chrono::milliseconds(2000));

	//Load image
	space = space_tex;

	//init stuff
	initEnemies();
	initPlayer();

	//font	
	video::SColor Color = video::SColor(255, 255, 255, 255);
	char textBuffer[64];
	sprintf_s(textBuffer, "SCORE: % 05d", score);

	//Play the music
	//PlayMusicStream(music);
	//SetMusicLoopCount(music, -1);

	std::cout << "Hello World!\n";

	gui::IGUIStaticText* textElement = guienv->addStaticText(convertWString(textBuffer).c_str(), rect<s32>(0, 0, 260, 24), false, true, 0, -1, false);
	textElement->setRelativePosition(rect<s32>(screen_width - textElement->getTextWidth() - 20, 20, screen_width, screen_height));
	textElement->setOverrideColor(video::SColor(255, 255, 255, 255));

	u32 then = device->getTimer()->getTime();
	bool prevSpaceState = receiver.KeyDown(KEY_KEY_W);
	bool prevReturnState = receiver.KeyDown(KEY_RETURN);
	while (device->run())
	{
		//UpdateMusicStream(music);        // Update music buffer with new stream data

		// Work out a frame delta time.
		const u32 now = device->getTimer()->getTime();
		DeltaTime = (f32)(now - then) / 1000.f; // (clock() / CLOCKS_PER_SEC) - lastTick;
		then = now;

		//Handle events on queue
		if (receiver.KeyDown(irr::KEY_SPACE) && !prevSpaceState && player.alive == 1)
		{
			addBullet(player.pos.X + player.tex->getSize().Width / 2 - 3, player.pos.Y);
			//PlaySound(snd_blaster);
		}

		if (receiver.KeyDown(irr::KEY_RETURN) && !prevReturnState) {
			restart();
		}

		driver->beginScene(true, true, video::SColor(0, 0, 0, 255));

		updateExplo();
		updateBullet();
		updateEnemyBullet();
		updateEnemies();

		if (player.alive == 1)
			updatePlayer(); //player

		updateLogic();

		driver->draw2DImage(space, position2d<s32>(0, 0));

		drawExplo();
		drawEnemies();
		drawBullet();
		drawEnemyBullet();

		if (player.alive == 1)
		{
			driver->draw2DImage(player.tex, position2d<s32>(player.pos.X, player.pos.Y), rect<s32>(0, 0, player.size.X, player.size.Y), 0, video::SColor(255, 255, 255, 255), true);
			//driver->draw2DRectangle(video::SColor(255, 255, 255, 255), rect<s32>(player.pos.X, player.pos.Y, player.pos.X+32, player.pos.Y+32));
		}
		else
		{
			driver->draw2DImage(gameover_tex, position2d<s32>(0, 0), rect<s32>(0, 0, gameover_tex->getSize().Width, gameover_tex->getSize().Height), 0, video::SColor(255, 255, 255, 255), true);
		}

		if (enemies_killed <= 0)
		{
			driver->draw2DImage(win_tex, position2d<s32>(0, 0), rect<s32>(0, 0, win_tex->getSize().Width, win_tex->getSize().Height), 0, video::SColor(255, 255, 255, 255), true);
		}

		//this ugly block is updating the score
		sprintf_s(textBuffer, "SCORE: % 05d", score);
		textElement->setText(convertWString(textBuffer).c_str());

		//lastTick = device->getTimer()->getTime() / 1000.0f; //(clock() / CLOCKS_PER_SEC);
		std::this_thread::sleep_for(std::chrono::milliseconds(30));

		// Update the previous state for the next frame
		prevSpaceState = receiver.KeyDown(irr::KEY_SPACE);
		prevReturnState = receiver.KeyDown(irr::KEY_RETURN);
		guienv->drawAll();

		driver->endScene();
	}

	device->drop();
}
