#include <stdio.h>
#include "util.h"
#include "smgbalib.h"

#include "BoyScout.h"	// Include this file in the files in which you want BoyScout playback capabilities.
#include "GBASoundRegs.h" // Internally used file to manage GameBoy Advance sound registers.

// this is generated with pcx2gba.exe from a indexed colour pcx image
#include "rd\bullet.h"
#include "rd\ebullet.h"
#include "rd\explode.h"
#include "rd\fmg.h"
#include "rd\invader.h"
#include "rd\player.h"
#include "rd\space3.h"
#include "rd\game_color_palette.h"
#include "rd\pusher.h"
#include "rd\music.h"

float Timestep = 0;
float lastdelta = 0;
float curTime = 0;

// current video frame counter (increments every time the screen refreshes)
u16 currentFrame = 0;

struct player_t {
    struct rectangle hitbox;
    struct vector2 pos;
    u16 *tex;
    int alive; //bool
};

struct bullet_t
{
    struct rectangle hitbox;
    struct vector2 pos;
    u16 *tex;
};

struct enemy_bullet_t
{
    struct rectangle hitbox;
    struct vector2 pos;
    u16 *tex;
};

struct explo_t
{
    struct vector2 pos;
    struct rectangle rect;
    struct rectangle size;
    u16 *tex;
    int animationFinished; //bool
    float currentFrame_ex;
};

struct enemy_t {
    struct rectangle hitbox;
    struct vector2 pos;
    struct rectangle rect;
    u16 *tex;
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

char textBuffer[16];
char textBuffer0[16];
char textBuffer1[16];
int score = 0;

int start_game = 0; //bool
float gameSessionTime = 0.f; // time since start

//snd channels
/*
SAMPLE *blaster_snd;
SAMPLE *pusher_snd;
SAMPLE *explode_snd;
MIDI *music;
*/

int A_BUTTON = 0; // button state bool
int A_BUTTON_DOWN = 0; // bool

u16 *enemy_tex = NULL;
u16 *player_tex = NULL;
u16 *bullet_tex = NULL;
u16 *enemy_bullet_tex = NULL;
u16 *explo_tex = NULL;
u16 *fmg_tex = NULL;
u16 *space_tex = NULL;
u16 *fontTex = NULL;

void preload_asset()
{
    //fmg_tex = (u16*)fmgData;
    space_tex = (u16*)space3Data;
    enemy_bullet_tex = (u16*)ebulletData;
    enemy_tex = (u16*)invaderData;
    bullet_tex = (u16*)bulletData;
    player_tex = (u16*)playerData;
    explo_tex = (u16*)explodeData;

    //fontTex = load_bitmap("/rd/font.bmp", NULL);
    /*
    blaster_snd = load_wav("/rd/blaster.wav");
    pusher_snd = load_wav("/rd/pusher.wav");
    explode_snd = load_wav("/rd/explode1.wav");
    music = load_midi("/rd/utr.mid");
    //music = load_sample("/rd/mus.wav");
    */
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
        enemy_bullets[i]->hitbox.w = 8;
        enemy_bullets[i]->hitbox.h = 8;
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
            enemy_bullets[i]->pos.y += 5;// * Timestep;

            if (enemy_bullets[i]->pos.y >= 160 - 9)
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
          DrawBitmapTransparent(enemy_bullets[i]->pos.x, enemy_bullets[i]->pos.y, ebullet_WIDTH, ebullet_HEIGHT, enemy_bullet_tex);
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
        enemy[i]->rect.h = 16;
        enemy[i]->rect.w = 16;
        enemy[i]->rect.x = 0;
        enemy[i]->rect.y = 0;
        enemy[i]->pos.x = (-25) + itemCount * 15;
        enemy[i]->pos.y = (80) - enemy[i]->rect.w - (20 * rowCount);
        enemy[i]->startPos = enemy[i]->pos.x;
        enemy[i]->rowPosID = 10 * (11 - itemCount);
        enemy[i]->goLeft = 0;
        enemy[i]->hitbox.w = 16;
        enemy[i]->hitbox.h = 16;
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
            enemy[e]->currentFrame += 1 * (Timestep/10);
            enemy[e]->rect.x = (int)enemy[e]->currentFrame * 16;

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
                enemy[e]->pos.x += moveSpeed;// * (Timestep);
            }

            if (enemy[e]->goLeft == 1)
            {
                enemy[e]->pos.x -= moveSpeed;// * (Timestep);
            }

            if (enemy[e]->pos.x >= (enemy[e]->startPos + enemy[e]->rect.w + 40) && enemy[e]->goLeft == 0)
            {
                enemy[e]->goLeft = 1;
            }

            if (enemy[e]->pos.x <= enemy[e]->startPos + enemy[e]->rect.w && enemy[e]->goLeft == 1)
            {
                enemy[e]->goLeft = 0;
            }

            enemy[e]->shootTimer += 1 * Timestep/10;

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
                addEnemyBullet(enemy[e]->pos.x + enemy[e]->rect.w / 2 - 4, enemy[e]->pos.y + 4);
                //play_sample(pusher_snd, 255, 0, 1000, 0);
				PlayVolumeSweep(WHITE_NOISE_SPLASH);
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
                //DrawBitmapSectorTransparent(enemy[e]->pos.x, enemy[e]->pos.y, invader_WIDTH, invader_HEIGHT, 0, 0, enemy[e]->hitbox.w, enemy[e]->hitbox.h, enemy_tex);
                DrawBitmapTransparent(enemy[e]->pos.x, enemy[e]->pos.y, invader_WIDTH, invader_HEIGHT, enemy_tex);
            }
        }
			/*
			if (enemy[0]->pos.x >= 10 - (enemy[0]->rect.w + enemy[0]->rowPosID) && enemy[0]->goLeft == 0)
            {
				WriteStringWide( 0, 0, "LEFT");
            }

            if (enemy[0]->pos.x <= enemy[0]->startPos + enemy[0]->rect.w && enemy[0]->goLeft == 1)
            {
				WriteStringWide( 0, 0, "RIGHT");
            }
			*/
			
			/*
			sprintf(textBuffer0, "% 05d", enemy[1]->startPos );// enemy[0]->pos.x);
			WriteStringWide( 0, 0, textBuffer0 );
			
			sprintf(textBuffer1, "% 05d", enemy[1]->pos.x );// enemy[0]->pos.x);
			WriteStringWide( 0, 10, textBuffer1 );
			//WriteStringWide( 0, 0, enemy[0]->startPos );
			*/
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
        bullets[i]->hitbox.w = 8;
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
            bullets[i]->pos.y -= 15 * Timestep;

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
          DrawBitmapTransparent(bullets[i]->pos.x, bullets[i]->pos.y, bullet_WIDTH, bullet_HEIGHT, bullet_tex);
        }
}

/*
Player
*/

void initPlayer()
{
    player.tex = player_tex;
    player.hitbox.w = 16;
    player.hitbox.h = 16;
    p_move = 130 / 2 - player.hitbox.w / 2;
    player.pos.y = (160 - 16) - player.hitbox.h / 2;
    player.hitbox.x = player.pos.x;
    player.hitbox.y = player.pos.y;
    player.alive = 1;
}

void input()
{
    //controls

    if (IS_B_PRESSED)
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
        addBullet(player.pos.x + (player.hitbox.w / 2) - 6, player.pos.y);
        //play_sample(blaster_snd, 255, 0, 1000, 0);
		PlayFrequencySweep(SWEEP_FREQUENCY_HIGHER);
        A_BUTTON_DOWN = 1;
    }


    if (IS_LEFT_PRESSED) {
        //printf("LEFT! \r\n");
        p_move -= 5;
    }

    if (IS_RIGHT_PRESSED ) {
        //printf("RIGHT! \r\n");
        p_move += 5;
    }

//  }

}

void updatePlayer()
{
    input();

    player.pos.x = p_move;

    player.hitbox.x = player.pos.x;
    player.hitbox.y = player.pos.y;

    if (player.pos.x <= 5)
    {
        player.pos.x = 5;
    }
    else if (player.pos.x >= 125 - player.hitbox.w)
    {
        player.pos.x = 125 - player.hitbox.w;
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
            explo[e]->currentFrame_ex += 3 * Timestep;
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
          //DrawBitmapSectorTransparent(explo[i]->pos.x, explo[i]->pos.y, explode_WIDTH, explode_HEIGHT, 0, 0, explo[i]->rect.w, explo[i]->rect.h, explo_tex);
          	DrawBitmapTransparent( explo[i]->pos.x, explo[i]->pos.y, explode_WIDTH, explode_HEIGHT, (u16*)explo_tex);
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
                        addExplo(bullets[i]->pos.x - 8 / 2, bullets[i]->pos.y - 8 / 2);
                        removeBullet(i);
                        //play_sample(explode_snd, 255, 0, 1000, 0);
						PlayWhiteNoise( WHITE_NOISE_EXPLOSION_LOW );
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
                addExplo(player.pos.x - 8 / 2, player.pos.y - 8 / 2);
                removeEnemyBullet(i);
                //play_sample(explode_snd, 255, 0, 1000, 0);
				PlayWhiteNoise( WHITE_NOISE_EXPLOSION_LOW );
                //printf("BOOM!\n");
                break;
            }
        }

}

int main() {

    // get ready for some displayin'
	Initialize();
	
	unsigned int nBSSongSize;
	
	BoyScoutInitialize( );

    //SetPalette((u16*)space3Palette);

    preload_asset();
	
	nBSSongSize  = BoyScoutGetNeededSongMemory(musicData); // Get needed song memory
	
	BoyScoutSetMemoryArea((unsigned int)malloc(nBSSongSize)); // Allocate and set BoyScout memory area	
	BoyScoutOpenSong(musicData); // Open song	
//	BoyScoutPlaySong(1); // Play song and loop

    // original res 640x480
    // GBA res 240x160
    struct rectangle std_res;
    std_res.w = 240;
    std_res.h = 160;

    initEnemies();
    initPlayer();

    //play_midi(music, TRUE);
	
    SetPalette((u16*)fmgPalette);	
	

    while (1)
    {
      curTime = currentFrame;
      //Timestep = curTime - lastdelta;
      Timestep = 1;

        gameSessionTime += 1 * Timestep/10;

        if (gameSessionTime >= 3.f)
        {
            start_game = 1;
        }

        if (start_game == 0) {
            DrawFullScreenOpaque((u16*)fmgData);
            Flip();
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

            DrawFullScreenOpaque((u16*)space_tex);

            drawExplo();
            drawEnemies();
            drawBullet();
            drawEnemyBullet();

            sprintf(textBuffer, "SCORE: % 05d", score);
            WriteStringWide( 180, 5, textBuffer);

            if (player.alive == 1) {
              DrawBitmapTransparent(player.pos.x, player.pos.y, player_WIDTH, player_HEIGHT, (u16*)player_tex);
            }
            else
            {
              WriteStringWide( 40, 72, "GAME OVER!" );
            }

            if(score == MAX_ENEMIES*100)
              WriteStringWide( 40, 72, "YOU WIN!" );

            // flip buffers, bringing the back one (on which we've been drawring) to the screen
		        Flip();

        }

        //clean up
		
        lastdelta = curTime;	
		
	  WaitForVblank();
	  BoyScoutUpdateSong() *BoyScoutGetSpeed(); // Update song
    }

    return 0;
}
