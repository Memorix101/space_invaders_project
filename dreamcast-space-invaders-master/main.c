#include <kos.h>
#include <stdio.h>
#include <mp3/sndserver.h>
#include <math.h>
#include <zlib/zlib.h>
#include <dc/fmath.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include "glfx.h"
#include "timer.h"

/* You can safely remove this line if you don't use a ROMDISK */
extern uint8 romdisk[];
KOS_INIT_ROMDISK(romdisk);

semaphore_t sem;

extern GLuint glTextureLoadPVR(char *fname, unsigned char isMipMapped, unsigned char glMipMap);

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
int enemies_killed = MAX_ENEMIES;

//stuff
struct bullet_t *bullets[MAX_BULLETS] = { NULL };
struct enemy_bullet_t * enemy_bullets[MAX_ENEMY_BULLETS] = { NULL };
#define MAX_EXPLO 100
struct explo_t *explo[MAX_EXPLO] = { NULL };
int rowCount = 0;
int itemCount = 0;
int lastTick;

char textBuffer[64];
int score = 0;

int start_game = 0; //bool
float gameSessionTime = 0.f; // time since start

int vmu_task = 0; //bool 

//snd channels
int sample1_channel = 0;
int sample2_channel = 10;
int sample3_channel = 20;

sfxhnd_t sample1 = NULL;
sfxhnd_t sample2 = NULL;
sfxhnd_t sample3 = NULL;

//dc controller
maple_device_t *cont;
cont_state_t *state;

int A_BUTTON = 0; // button state bool
int A_BUTTON_DOWN = 0; // bool

GLuint enemy_tex;
GLuint player_tex;
GLuint bullet_tex;
GLuint enemy_bullet_tex;
GLuint explo_tex;
GLuint fmg_splash;
GLuint space;
GLuint fontTex;

//setup vmu display image
#include "vmu.xpm"

/* LCD Test: this will do a grayscale seperation into several "frames" and
   flip through them quickly to give the illusion of grayscale on the LCD
   display. */
uint8 lcd_disp[8][48 * 32 / 8];
void lcd_gs_pixel(int x, int y, int amt) {
    int i;

    for(i = 0; i < amt; i++)
        lcd_disp[i][(y * 48 + x) / 8] |= 0x80 >> (x & 7);
}
void lcd_gs_setup() {
    char **xpm = graphic_xpm + 12;  /* Skip header */
    int x, y;

    memset(lcd_disp, 0, sizeof(lcd_disp));

    for(y = 0; y < 32; y++) {
        for(x = 0; x < 48; x++) {
            /* Note that LCD images must be flipped! */
            int pixel = xpm[31 - y][47 - x];

            switch(pixel) {
                case '.':   /* White */
                    lcd_gs_pixel(x, y, 0);
                    break;
                case '#':   /* #e3e3e3 */
                    lcd_gs_pixel(x, y, 1);
                    break;
                case '+':   /* #c7c7c7 */ 
                    lcd_gs_pixel(x, y, 2);
                    break;
                case '@':   /* #aaaaaa */
                    lcd_gs_pixel(x, y, 3);
                    break;
                case '=':   /* #8e8e8e */
                case '-':   /* #727272 */
                    lcd_gs_pixel(x, y, 4);
                    break;
                case '$':   /* #555555 */
                    lcd_gs_pixel(x, y, 5);
                    break;
                case '*':   /* #393939 */
                    lcd_gs_pixel(x, y, 6);
                    break;
                case '&':   /* #1d1d1d */
                case '%':   /* #000000 */
                    lcd_gs_pixel(x, y, 7);
                    break;
            }
        }
    }
}

/* This performs the actual magic */
void lcd_test() {

    lcd_gs_setup();

    while(vmu_task == 0) {
        maple_device_t *addr = maple_enum_type(0, MAPLE_FUNC_LCD);

        if(addr) {
            int rv = vmu_draw_lcd(addr, lcd_disp);

            if(rv < 0)
                printf("got error %d\n", rv);
            else {
             //  printf("mounted vmu successfully %d\n", rv);
              vmu_task = 1;
              break;
            }
        }
    }
}

void preload_asset()
{

  fmg_splash = glTextureLoadPVR("/rd/fmg_splash.pvr", 0, 0);
  space = glTextureLoadPVR("/rd/space3.pvr", 0, 0);
  enemy_bullet_tex = glTextureLoadPVR("/rd/enemy-bullet.pvr", 0, 0);
  enemy_tex = glTextureLoadPVR("/rd/invader32x32x4.pvr", 0, 0);
  bullet_tex = glTextureLoadPVR("/rd/bullet.pvr", 0, 0);
  player_tex = glTextureLoadPVR("/rd/player.pvr", 0, 0);
  explo_tex = glTextureLoadPVR("/rd/explode.pvr", 0, 0);
  fontTex = glTextureLoadPVR("/rd/font.pvr", 0, 0);

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
    draw_sprite(enemy_bullets[i]->tex, enemy_bullets[i]->pos, enemy_bullets[i]->hitbox);
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
      sample2_channel = snd_sfx_play(sample2, 225, 128);
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
      draw_spritesheet(enemy[e]->tex, enemy[e]->pos, enemy[e]->hitbox, enemy[e]->rect, 128, 32);
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
    draw_sprite(bullets[i]->tex, bullets[i]->pos, bullets[i]->hitbox);
  }
}

/*
Player
*/

void initPlayer()
{
  player.tex = player_tex;
  player.hitbox.w = 28;
  player.hitbox.h = 21;
  p_move = 640 / 2 - player.hitbox.w / 2;
  player.pos.y = (480 - 60) - player.hitbox.h / 2;
  player.hitbox.x = player.pos.x;
  player.hitbox.y = player.pos.y;
  player.alive = 1;
}

void input()
{
  //dc controls
  cont = maple_enum_type(0, MAPLE_FUNC_CONTROLLER);

  if (cont) {
    state = (cont_state_t *)maple_dev_status(cont);

    if (state->buttons & CONT_A)
    {
      A_BUTTON = 1;
    }
    else
    {
      A_BUTTON = 0;
      A_BUTTON_DOWN = 0;
    }

    if (A_BUTTON == 1 && A_BUTTON_DOWN == 0)
    {
      //printf("SHOOT! \r\n");
      addBullet(player.pos.x + player.hitbox.w / 2 - 3, player.pos.y);
      sample1_channel = snd_sfx_play(sample1, 255, 128);
      A_BUTTON_DOWN = 1;
    }


    if (state->buttons & CONT_DPAD_LEFT || state->joyx <= -1) {
      //printf("LEFT! \r\n");
      p_move -= 350 * Timestep;
    }

    if (state->buttons & CONT_DPAD_RIGHT || state->joyx >= 1) {
      //printf("RIGHT! \r\n");
      p_move += 350 * Timestep;
    }

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
    draw_spritesheet(explo[i]->tex, explo[i]->pos, explo[i]->rect, explo[i]->rect, 2048, 128);
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
        sample3_channel = snd_sfx_play(sample3, 255, 128);
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
      addExplo(player.pos.x - 128 / 2, player.pos.y - 128 / 2);
      removeEnemyBullet(i);
      sample3_channel = snd_sfx_play(sample3, 255, 128);
      //printf("BOOM!\n");
      break;
    }
  }

}

int main(int argc, char *argv[]) {

  /* Get basic stuff initialized */
  glKosInit();

  snd_stream_init();
  mp3_init();
  snd_init();

  initGL(640, 480);

  lcd_test();

  preload_asset();

  struct rect std_res;
  std_res.w = 640;
  std_res.h = 480;

  struct rect r1;
  r1.w = 64;
  r1.h = 64;

  initEnemies();
  initPlayer();

  sample1 = snd_sfx_load("/rd/blaster.wav");
  sample2 = snd_sfx_load("/rd/pusher.wav");
  sample3 = snd_sfx_load("/rd/explode1.wav");

  mp3_start("/rd/bodenstaendig.mp3", 0);

  struct vector2 vz;
  vz.x = 0;
  vz.y = 0;

  struct rect rc;
  rc.w = 32;
  rc.h = 32;

  struct vector2 textpos;
  textpos.x = 640 - 200;
  textpos.y = 20;

  struct vector2 ctextpos;
  ctextpos.x = 640/2 - 240/2;
  ctextpos.y = 480/2 - 16;

  /* Loop until the user closes the window */
  while (1)
  {

    curTime = (float)GetTime() / 1000;
    Timestep = curTime - lastdelta;
    lastdelta = curTime;

    gameSessionTime += 1 * Timestep;

    if (gameSessionTime >= 3.f)
    {
      start_game = 1;
    }

    if (start_game == 0) {
      draw_sprite(fmg_splash, vz, std_res);
    }

    if (start_game == 1)
    {
      updateExplo();
      updateBullet();
      updateEnemyBullet();
      updateEnemies();

      if (player.alive == 1)
        updatePlayer(); //player

      updateLogic();

      draw_sprite(space, vz, std_res);
      drawExplo();
      drawEnemies();
      drawBullet();
      drawEnemyBullet();

      sprintf(textBuffer, "SCORE: % 05d", score);
      createFont(fontTex, 16, textpos, textBuffer);

      if(enemies_killed <= 0)
      createFont(fontTex, 32, ctextpos, "YOU WIN!");

      if (player.alive == 1) {
        draw_sprite(player.tex, player.pos, player.hitbox);
      }
      else
      {
        createFont(fontTex, 32, ctextpos, "GAME OVER!");
      }

    }


    glutSwapBuffers();

  }

  return 0;
}
