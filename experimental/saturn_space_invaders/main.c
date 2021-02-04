#include "util.h"
#include <jo/jo.h>
#include <stdio.h>

float Timestep = 0;
float lastdelta = 0;
float curTime = 0;

struct player_t {
  struct rect hitbox;
  struct vector2 pos;
  int tex;
  int alive; //bool
};

struct bullet_t
{
  struct rect hitbox;
  struct vector2 pos;
  int tex;
};

struct enemy_bullet_t
{
  struct rect hitbox;
  struct vector2 pos;
  int tex;
};

struct explo_t
{
  struct vector2 pos;
  struct rect rect;
  struct rect size;
  int tex;
  int animationFinished; //bool
  float currentFrame_ex;
};

struct enemy_t {
  struct rect hitbox;
  struct vector2 pos;
  struct rect rect;
  int tex;
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
#define MAX_ENEMIES 10
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
int gameover = 0;
int dead_enemies = 0;

char textBuffer[64];
int score = 0;

int start_game = 0; //bool
float gameSessionTime = 0.f; // time since start

static jo_sound blaster;
static jo_sound pusher;
static jo_sound explode1;
static jo_sound music;

static int enemy_tex;
static int player_tex;
static int bullet_tex;
static int enemy_bullet_tex;
static int explo_tex;
static int fmg_splash;
static int space_tex;
static int ui_win_tex;
static int ui_gameover_tex;
//static int fontTex;

int anim_invader;
int anim_explo;

/* preset stuff */
struct rect std_res;
struct rect r1;
struct vector2 vz;
struct rect rc;
struct vector2 textpos;
struct vector2 ctextpos;

void preload_assets(void)
{
  fmg_splash = jo_sprite_add_tga(JO_ROOT_DIR, "FMG.TGA", NULL);
  space_tex = jo_sprite_add_tga(JO_ROOT_DIR, "SPACE.TGA", NULL);
  ui_win_tex = jo_sprite_add_tga(JO_ROOT_DIR, "WIN.TGA", JO_COLOR_Green);
  ui_gameover_tex = jo_sprite_add_tga(JO_ROOT_DIR, "GAMEOVER.TGA", JO_COLOR_Green);
  enemy_bullet_tex = jo_sprite_add_tga(JO_ROOT_DIR, "EBULLET.TGA", JO_COLOR_Transparent);

  //anim invaders
  enemy_tex = jo_sprite_add_image_pack("INVADER", "INV.TEX", JO_COLOR_Green);
	anim_invader = jo_create_sprite_anim(enemy_tex, 4, 10);
	jo_start_sprite_anim_loop(anim_invader);

  //anim explode
/*explo_tex = jo_sprite_add_image_pack("EXPLODE", "EXPLO.TEX", JO_COLOR_Green);
  anim_explo = jo_create_sprite_anim(explo_tex, 16, 30);
  jo_start_sprite_anim(anim_explo);
*/

  bullet_tex = jo_sprite_add_tga(JO_ROOT_DIR, "BULLET.TGA", JO_COLOR_Transparent);
  player_tex = jo_sprite_add_tga(JO_ROOT_DIR, "PLAYER.TGA", JO_COLOR_Transparent);
  explo_tex = jo_sprite_add_tga(JO_ROOT_DIR, "EXPLO16.TGA", JO_COLOR_Green);
  //fontTex = jo_sprite_add_tga(JO_ROOT_DIR, "SPACE3.TGA", NULL);

  jo_audio_load_pcm("BLASTER.PCM", JoSoundStereo8Bit, &blaster);
  jo_audio_load_pcm("PUSHER.PCM", JoSoundStereo8Bit, &pusher);
  jo_audio_load_pcm("EXPLO1.PCM", JoSoundMono16Bit, &explode1);
}

void play_music(void)
{
  static int is_cd_playing = 0;
  if (!is_cd_playing)
  {
    /* The first two tracks in this demo are reserved so the first audio track is 3.
    For more details you can open "game.cue" file with a notepad */
    jo_audio_play_cd_track(3, 3, true);
    is_cd_playing = true;
  }
  else
  {
    jo_audio_stop_cd();
    is_cd_playing = false;
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
    enemy_bullets[i]->hitbox.w = 12;
    enemy_bullets[i]->hitbox.h = 12;
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
    enemy_bullets[i]->pos.y += 5;

    if (enemy_bullets[i]->pos.y >= 160)
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
    //draw_sprite(enemy_bullets[i]->tex, enemy_bullets[i]->pos, enemy_bullets[i]->hitbox);
    jo_sprite_draw3D(enemy_bullets[i]->tex, enemy_bullets[i]->pos.x, enemy_bullets[i]->pos.y, 500);
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
    enemy[i]->tex = anim_invader;//enemy_tex;
    enemy[i]->alive = 1;
    enemy[i]->rect.h = 32;
    enemy[i]->rect.w = 32;
    enemy[i]->rect.x = 0;
    enemy[i]->rect.y = 0;
    enemy[i]->pos.x = -220 +(itemCount * 40);
    enemy[i]->pos.y = -120 + (40 * rowCount);
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

    if (i % 5 == 0)
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

  const int moveSpeed = 3;

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

  //  if (enemy[e]->pos.x >= 160 - (enemy[e]->rect.w + enemy[e]->rowPosID) && enemy[e]->goLeft == 0)
   if (enemy[e]->pos.x >= 420 - (enemy[e]->rect.w + enemy[e]->rowPosID) && enemy[e]->goLeft == 0)
    {
      enemy[e]->goLeft = 1;
    }

    if (enemy[e]->pos.x <= enemy[e]->startPos + enemy[e]->rect.w && enemy[e]->goLeft == 1)
    {
      enemy[e]->goLeft = 0;
    }

    enemy[e]->shootTimer += 1 * Timestep/1000;

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
      jo_audio_play_sound_on_channel(&pusher, 0);
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
      //draw_spritesheet(enemy[e]->tex, enemy[e]->pos, enemy[e]->hitbox, enemy[e]->rect, 128, 32);
      jo_sprite_draw3D(jo_get_anim_sprite(enemy[e]->tex), enemy[e]->pos.x, enemy[e]->pos.y, 500);
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
    bullets[i]->hitbox.w = 32;
    bullets[i]->hitbox.h = 32;
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
    bullets[i]->pos.y -= 7;

    if (bullets[i]->pos.y <= -150)
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
    //draw_sprite(bullets[i]->tex, bullets[i]->pos, bullets[i]->hitbox);
      jo_sprite_draw3D(bullets[i]->tex, bullets[i]->pos.x, bullets[i]->pos.y, 500);
  }
}

/*
Player
*/

void initPlayer()
{
  player.tex = player_tex;
  player.hitbox.w = 32;
  player.hitbox.h = 32;
  p_move = 0;//320 / 2 - player.hitbox.w / 2;
  player.pos.y = 90;//(240 - 60) - player.hitbox.h / 2;
  player.hitbox.x = player.pos.x;
  player.hitbox.y = player.pos.y;
  player.alive = 1;
}

void input()
{
  //saturn controls
  if (!jo_is_pad1_available())
		return ;

    if (jo_is_pad1_key_down(JO_KEY_A))
    {
      //printf("SHOOT! \r\n");
      addBullet(player.pos.x + player.hitbox.w / 2 - 15, player.pos.y);
      jo_audio_play_sound_on_channel(&blaster, 1);
    }

    if (jo_is_pad1_key_pressed(JO_KEY_LEFT)) {
      //printf("LEFT! \r\n");
      p_move -= 5;// * Timestep;
    }

    if (jo_is_pad1_key_pressed(JO_KEY_RIGHT)) {
      //printf("RIGHT! \r\n");
      p_move += 5;// * Timestep;
    }
}

void updatePlayer()
{
  input();

  player.pos.x = p_move;

  player.hitbox.x = player.pos.x;
  player.hitbox.y = player.pos.y;

  if (player.pos.x <= -160 + player.hitbox.w)
  {
    player.pos.x = -160 + player.hitbox.w;
  }
  else if (player.pos.x >= 160 - player.hitbox.w)
  {
    player.pos.x = 160 - player.hitbox.w;
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
    explo[i]->rect.w = 32;
    explo[i]->rect.h = 32;
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
    explo[e]->rect.x = (int)explo[e]->currentFrame_ex * 32;

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
    //draw_spritesheet(explo[i]->tex, explo[i]->pos, explo[i]->rect, explo[i]->rect, 2048, 128);
    jo_sprite_draw3D(explo[i]->tex, explo[i]->pos.x, explo[i]->pos.y, 500);
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
        addExplo(bullets[i]->pos.x - 32 / 2, bullets[i]->pos.y - 32 / 2);
        removeBullet(i);
        jo_audio_play_sound_on_channel(&explode1, 2);
        score += 100;
        dead_enemies++;
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
      addExplo(player.pos.x - 32 / 2, player.pos.y - 32 / 2);
      removeEnemyBullet(i);
      jo_audio_play_sound_on_channel(&explode1, 2);
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
  for (int e = 0; e < MAX_ENEMIES; e++) if (enemy[e]) 
    enemy[e]->currentFrame = 0.0f;
	memset(bullets, 0, sizeof(bullets));
	memset(enemy_bullets, 0, sizeof(enemy_bullets));
	memset(explo, 0, sizeof(explo));
	rowCount = 0;
	itemCount = 0;
	initEnemies();
	p_move = 0;
	player.alive = 1;
}

void draw(void)
{
/*  curTime = time.second*1000; //(float)GetTime() / 1000;
  Timestep = curTime - lastdelta;
  lastdelta = curTime;
  */
  jo_datetime  time;
  jo_getdate(&time);

  Timestep = (int)time.second;

  gameSessionTime = Timestep;

  //jo_printf(0, 0, "Timestep %d", time.second);

  if (gameSessionTime >= 20.f)
  {
    start_game = 1;
  }

  if (start_game == 0) {
      jo_sprite_draw3D(fmg_splash, 0, 0, 500);
  }

  if (start_game == 1)
  {

    //jo_printf(0, 1, "Game is active!");
    //jo_printf(0, 1, "Dynamic memory usage: %d%%  ", jo_memory_usage_percent());
    //jo_printf(0, 2, "Sprite memory usage: %d%%  ", jo_memory_fragmentation());

    updateExplo();
    updateBullet();
    updateEnemyBullet();
    updateEnemies();

    if (player.alive == 1)
      updatePlayer(); //player

    updateLogic();

    jo_sprite_draw3D(space_tex, 0, 0, 500);

    drawExplo();
    drawEnemies();
    drawBullet();
    drawEnemyBullet();

    sprintf(textBuffer, "SCORE: % 05d", score);
    jo_printf(90, 0, textBuffer); //createFont(fontTex, 16, textpos, textBuffer);

    if(score >= 1000){
      //jo_printf(16, 15, "YOU WIN!"); //createFont(fontTex, 32, ctextpos, "YOU WIN!");
      //jo_printf(10, 18, "PRESS START TO RESTART");
      jo_sprite_draw3D(ui_win_tex, 0, 0, 500);
      gameover = 1;
    }

    if (player.alive == 1) {
      //draw_sprite(player.tex, player.pos, player.hitbox);
      jo_sprite_draw3D(player.tex, player.pos.x, player.pos.y, 500);
    }
    else
    {
      //jo_printf(15, 15,  "GAME OVER!"); //createFont(fontTex, 32, ctextpos, "GAME OVER!");
      //jo_printf(10, 18, "PRESS START TO RESTART");
      jo_sprite_draw3D(ui_gameover_tex, 0, 0, 500);
      gameover = 1;
    }

    if (jo_is_pad1_key_down(JO_KEY_START) && gameover == 1) {
      reset();
    }
  }
}

void jo_main(void) {

  jo_core_init(JO_COLOR_Black);
	preload_assets();

  initEnemies();
  initPlayer();

  jo_core_add_callback(draw);
  jo_core_add_callback(play_music);
  jo_core_run();

  return 0;
}
