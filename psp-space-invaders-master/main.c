#include <pspkernel.h>
#include <pspdebug.h>
#include <pspdisplay.h>
#include <pspctrl.h>
#include <stdio.h>
#include <string.h>
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_image.h>
#include <time.h>

#include "./common/callbacks.h"
#include "./common/vram.h"

struct player_t {
  SDL_Rect hitbox;
  SDL_Rect pos;
  SDL_Surface * tex;
  int alive; //bool
};

struct bullet_t
{
  SDL_Rect hitbox;
  SDL_Rect pos;
  SDL_Surface * tex;
};

struct enemy_bullet_t
{
  SDL_Rect hitbox;
  SDL_Rect pos;
  SDL_Surface * tex;
};

struct explo_t
{
  SDL_Rect pos;
  SDL_Rect rect;
  SDL_Surface * tex;
  int animationFinished; //bool
  int currentFrame_ex;
};

struct enemy_t {
  SDL_Rect hitbox;
  SDL_Rect pos;
  SDL_Rect rect;
  SDL_Surface * tex;
  int goLeft; //bool
  int startPos;
  int rowPosID;
  int alive; //bool
  float shootTimer;
  int shoot; //bool
  float shootTimeLimit;
  float currentFrame;
};

//base SDL stuff
SDL_Event e;
SDL_Surface* screen = NULL;

//player
#define MAX_BULLETS 50
struct player_t player;
int p_move;

//enemy
#define MAX_ENEMY_BULLETS 100
#define MAX_ENEMIES 30
struct enemy_t *enemy[MAX_ENEMIES] = { NULL };
int enemies_killed = MAX_ENEMIES;

//stuff
struct bullet_t *bullets[MAX_BULLETS] = { NULL };
struct enemy_bullet_t *enemy_bullets[MAX_ENEMY_BULLETS] = { NULL };
#define MAX_EXPLO 100
struct explo_t *explo[MAX_EXPLO] = { NULL };
int rowCount = 0;
int itemCount = 0;
char textBuffer[64];
int score = 0;
int gameover = 0;
int quit = 0;

//time
float Timestep = 0;
float lastdelta = 0;
float curTime = 0;
float prewarm = 0;

//Music
Mix_Music *music = NULL;
Mix_Chunk *snd_pusher = NULL;
Mix_Chunk *snd_blaster = NULL;
Mix_Chunk *snd_explo = NULL;

#define VERS    1 //Talk about this
#define REVS    0

PSP_MODULE_INFO("Space Invaders", PSP_MODULE_USER, VERS, REVS);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER);
PSP_HEAP_SIZE_MAX();

#define printf pspDebugScreenPrintf

//psp input
SceCtrlData pad;
int A_BUTTON = 0; // button state bool
int A_BUTTON_DOWN = 0; // bool


//preload assets
SDL_Surface * enemy_tex = NULL;
SDL_Surface * player_tex = NULL;
SDL_Surface * bullet_tex = NULL;
SDL_Surface * enemy_bullet_tex = NULL;
SDL_Surface * explo_tex = NULL;
SDL_Surface * fmg_splash_tex = NULL;
SDL_Surface * space_tex = NULL;
SDL_Surface*  fontTex = NULL;
SDL_Surface* gameover_tex2D = NULL;
SDL_Surface* win_tex2d = NULL;

void preload_asset()
{
  fmg_splash_tex = IMG_Load("rd/fmg_splash.png");
  space_tex = IMG_Load("rd/space3.png");
  enemy_bullet_tex = IMG_Load("rd/enemy-bullet.png");
  enemy_tex = IMG_Load("rd/invader32x32x4.png"); //invader32x32x4
  bullet_tex = IMG_Load("rd/bullet.png");
  player_tex = IMG_Load("rd/player.png");
  explo_tex = IMG_Load("rd/explode.png");
  fontTex = IMG_Load("rd/font.png");
  gameover_tex2D = IMG_Load("rd/gameover_ui.png");
	win_tex2d = IMG_Load("rd/win_ui.png");
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
  int i;
  for (i = 0; i < MAX_ENEMY_BULLETS; i++)
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
    enemy_bullets[i]->tex = enemy_bullet_tex;
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
    enemy_bullets[i]->pos.y += 350 * Timestep;

    if (enemy_bullets[i]->pos.y >= 272 - 9)
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
  int i;
  for (i = 0; i < MAX_ENEMIES; i++)
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
    enemy[i]->tex = enemy_tex;
    enemy[i]->alive = 1;
    enemy[i]->rect.h = 24;
    enemy[i]->rect.w = 24;
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
    enemy[e]->rect.x = (int)enemy[e]->currentFrame * 24;

    if (enemy[e]->currentFrame >= 3)
    {
      enemy[e]->currentFrame = 0;
    }
  }
}

void updateEnemies()
{
  animatorEnemies();

  const int moveSpeed = 100;
 // if (prewarm >= 5) {
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

      if (enemy[e]->pos.x >= 480 - (enemy[e]->rect.w + enemy[e]->rowPosID) && enemy[e]->goLeft == 0)
      {
        enemy[e]->goLeft = 1;
      }

      if (enemy[e]->pos.x <= enemy[e]->startPos + enemy[e]->rect.w && enemy[e]->goLeft == 1)
      {
        enemy[e]->goLeft = 0;
      }

      enemy[e]->shootTimer += 1 * Timestep;

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
 // }
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
Bullet
*/

void addBullet(float x, float y)
{
  int found = -1;
  int i;
  for (i = 0; i < MAX_BULLETS; i++)
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
    bullets[i]->tex = bullet_tex;
    bullets[i]->pos.x = x;
    bullets[i]->pos.y = y;
    bullets[i]->hitbox.w = bullets[i]->tex->w;
    bullets[i]->hitbox.h = bullets[i]->tex->h;;
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
    bullets[i]->pos.y -= 350 * Timestep;

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
Player
*/

void initPlayer()
{
  player.tex = player_tex;
  p_move = 480 / 2 - player.tex->w / 2;
  player.pos.y = (272 - 20) - player.tex->h / 2;
  player.hitbox.w = player.tex->w;
  player.hitbox.h = player.tex->h;
  player.hitbox.x = player.pos.x;
  player.hitbox.y = player.pos.y;
  player.alive = 1;
}

void input()
{

  if (pad.Buttons & PSP_CTRL_CROSS || pad.Buttons & PSP_CTRL_RTRIGGER)
  {
    A_BUTTON = 1;
  }
  else
  {
    A_BUTTON = 0;
    A_BUTTON_DOWN = 0;
  }

  if (A_BUTTON == 1 && A_BUTTON_DOWN == 0 && player.alive == 1)
  {
    //printf("SHOOT! \r\n");
    addBullet(player.pos.x + player.hitbox.w / 2 - 2, player.pos.y - 28);
    Mix_PlayChannel(-1, snd_blaster, 0);
    A_BUTTON_DOWN = 1;
  }

  if (pad.Buttons & PSP_CTRL_LEFT) {
    //printf("LEFT! \r\n");
    p_move -= 350 * Timestep;
  }

  if (pad.Buttons & PSP_CTRL_RIGHT) {
    //printf("RIGHT! \r\n");
    p_move += 350 * Timestep;
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
  else if (player.pos.x >= 480 - player.tex->w)
  {
    player.pos.x = 480 - player.tex->w;
  }
}

/*
Explosion
*/

void addExplo(float x, float y)
{
  int found = -1;
  int i;
  for (i = 0; i < MAX_EXPLO; i++)
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
    explo[i]->tex = explo_tex;
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
  int e;
  for (e = 0; e < MAX_EXPLO; e++) if (explo[e]) {
    explo[e]->currentFrame_ex += 25.0f * Timestep;
    explo[e]->rect.x = (int)explo[e]->currentFrame_ex * 64;

    if (explo[e]->currentFrame_ex >= 16)
    {
      explo[e]->animationFinished = 1;
    }

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

void updateExplo()
{
  animatorExplo();

  int i;
  for (i = 0; i < MAX_EXPLO; i++) if (explo[i])
  {
    if (explo[i]->animationFinished == 1)
    {
      //removeExplo(explo[i]);
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
    if (explo[i]->animationFinished == 0)
      SDL_BlitSurface(explo[i]->tex, &explo[i]->rect, screen, &explo[i]->pos);
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
        addExplo(bullets[i]->pos.x - 64 / 2, bullets[i]->pos.y - 64 / 2);
        removeBullet(i);
        Mix_PlayChannel(0, snd_explo, 0);
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
    if (enemy_bullets[b]->pos.x > player.pos.x && enemy_bullets[b]->pos.x < player.pos.x + player.hitbox.w &&
      enemy_bullets[b]->pos.y > player.pos.y && enemy_bullets[b]->pos.y < player.pos.y + player.hitbox.h &&
      player.alive == 1)
    {
      player.alive = 0;
      addExplo(player.pos.x - 64 / 2, player.pos.y - 64 / 2);
      removeEnemyBullet(i);
      Mix_PlayChannel(0, snd_explo, 0);
      //printf("BOOM!\n");
      break;
    }
  }

}

void reset()
{
	gameover = 0;
	score = 0;
  enemies_killed = MAX_ENEMIES;
	memset(enemy, 0, sizeof(enemy));
	memset(bullets, 0, sizeof(bullets));
	memset(enemy_bullets, 0, sizeof(enemy_bullets));
	memset(explo, 0, sizeof(explo));
	rowCount = 0;
	itemCount = 0;
	initEnemies();
	p_move = 640 / 2 - player.tex->w / 2;
	player.alive = 1;
}

int main(int argc, char *argv[]) {

  pspDebugScreenInit();

  //Start SDL
  SDL_Init(SDL_INIT_VIDEO);
  SDL_ShowCursor(SDL_DISABLE);

  //Initialize SDL_mixer
  if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 256) == -1)
  {
    return 0;
  }

  //Set up screen
  screen = SDL_SetVideoMode(480, 272, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);

  //preload assets
  preload_asset();
  music = Mix_LoadMUS("rd/bodenstaendig.ogg");
  snd_blaster = Mix_LoadWAV("rd/blaster.ogg");
  snd_explo = Mix_LoadWAV("rd/explode1.ogg");
  snd_pusher = Mix_LoadWAV("rd/pusher.ogg");

  SDL_BlitSurface(fmg_splash_tex, NULL, screen, NULL);
  SDL_Flip(screen);
  SDL_Delay(2000);
  SDL_FreeSurface(fmg_splash_tex);

  //setup fonts and stuff 
  SDL_Rect textpos;
  textpos.x = 480 - 200;
  textpos.y = 10;
  textpos.w = 0;
  textpos.h = 0;

  SDL_Rect ctextpos;
  ctextpos.x = 480/2 - 75;
  ctextpos.y = 272/2 - 8;
  ctextpos.w = 0;
  ctextpos.h = 0;

  SDL_Rect wctextpos;
  wctextpos.x = 480/2 - 60;
  wctextpos.y = 272/2 - 8;
  wctextpos.w = 0;
  wctextpos.h = 0;

  //calling the actors and the main cast
  initEnemies();
  initPlayer();

  //Play the music
  if (Mix_PlayMusic(music, -1) == -1)
  {
    return 1;
  }

  while (quit == 0)
  {
    curTime = (float)SDL_GetTicks()/1000.0f;
    Timestep = curTime - lastdelta;

    prewarm += 1 * Timestep;

    //Handle events on queue
    while (SDL_PollEvent(&e) != 0)
    {
      switch (e.type)
      {

      }
    }

    //psp controls
    sceCtrlReadBufferPositive(&pad, 1);

    updateExplo();
    updateBullet();
    updateEnemyBullet();
    updateEnemies();

    if (player.alive == 1)
      updatePlayer(); //player

    updateLogic();

    SDL_BlitSurface(space_tex, NULL, screen, NULL);
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
        //createFont(fontTex, 16, ctextpos, "GAME OVER!");
      }

      sprintf(textBuffer, "SCORE:%05d", score);
      createFont(fontTex, 16, textpos, textBuffer);

      if(enemies_killed <= 0){
        //createFont(fontTex, 16, wctextpos, "YOU WIN!");
        SDL_BlitSurface(win_tex2d, NULL, screen, NULL);
			  gameover = 1;
      }        

/*
    pspDebugScreenSetXY(0, 0);
    printf("textpos: %d", textpos.x);
    sceDisplayWaitVblankStart();
*/
    //Update Screen
    SDL_Flip(screen);

    if (pad.Buttons & PSP_CTRL_START && gameover == 1)
    {
      reset();
    }

    if (pad.Buttons & PSP_CTRL_HOME) {
      quit = 1;
      sceKernelExitGame();
    }
    lastdelta = curTime;
  }

  SDL_FreeSurface(enemy_tex);
	SDL_FreeSurface(player_tex);
	SDL_FreeSurface(bullet_tex);
	SDL_FreeSurface(enemy_bullet_tex);
	SDL_FreeSurface(explo_tex);
	SDL_FreeSurface(fmg_splash_tex);
	SDL_FreeSurface(space_tex);
	SDL_FreeSurface(fontTex);
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

  sceKernelExitGame();

  return 0;
}