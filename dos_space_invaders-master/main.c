#include <stdio.h>
#include <allegro.h>
#include "util.h"

float Timestep = 0;
float lastdelta = 0;
float curTime = 0;

struct player_t {
    struct rectangle hitbox;
    struct vector2 pos;
    BITMAP *tex;
    int alive; //bool
};

struct bullet_t
{
    struct rectangle hitbox;
    struct vector2 pos;
    BITMAP *tex;
};

struct enemy_bullet_t
{
    struct rectangle hitbox;
    struct vector2 pos;
    BITMAP *tex;
} ENEMY_BULLET;

struct explo_t
{
    struct vector2 pos;
    struct rectangle rect;
    struct rectangle size;
    BITMAP *tex;
    int animationFinished; //bool
    float currentFrame_ex;
};

struct enemy_t {
    struct rectangle hitbox;
    struct vector2 pos;
    struct rectangle rect;
    BITMAP *tex;
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
#define MAX_ENEMIES 20
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

//snd channels
SAMPLE *blaster_snd;
SAMPLE *pusher_snd;
SAMPLE *explode_snd;
MIDI *music;

int A_BUTTON = 0; // button state bool
int A_BUTTON_DOWN = 0; // bool

BITMAP *enemy_tex = NULL;
BITMAP *player_tex = NULL;
BITMAP *bullet_tex = NULL;
BITMAP *enemy_bullet_tex = NULL;
BITMAP *explo_tex = NULL;
BITMAP *fmg_splash = NULL;
BITMAP *space = NULL;
BITMAP *fontTex = NULL;

BITMAP *buffer;

volatile unsigned int ticks = 0;
void Ticker()
{
    ticks++;
}
END_OF_FUNCTION(Ticker)

void preload_asset()
{
    fmg_splash = load_tga("/rd/fmg.tga", NULL);
    space = load_tga("/rd/space3.tga", NULL);
    enemy_bullet_tex = load_tga("/rd/ebullet.tga", NULL);
    enemy_tex = load_tga("/rd/invader.tga", NULL);
    bullet_tex = load_tga("/rd/bullet.tga", NULL);
    player_tex = load_tga("/rd/player.tga", NULL);
    explo_tex = load_bitmap("/rd/explode.tga", NULL);

    //fontTex = load_bitmap("/rd/font.bmp", NULL);

    blaster_snd = load_wav("/rd/blaster.wav");
    pusher_snd = load_wav("/rd/pusher.wav");
    explode_snd = load_wav("/rd/explode1.wav");
    music = load_midi("/rd/utr.mid");
    //music = load_sample("/rd/mus.wav");
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
            enemy_bullets[i]->pos.y += 2 * Timestep;

            if (enemy_bullets[i]->pos.y >= 200 - 9)
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
            //void blit(BITMAP *source, BITMAP *dest, int source_x, int source_y, int dest_x, int dest_y, int width, int height);
            blit(enemy_bullets[i]->tex, buffer, 0, 0, enemy_bullets[i]->pos.x, enemy_bullets[i]->pos.y, enemy_bullets[i]->hitbox.x, enemy_bullets[i]->hitbox.y);
            //clear_bitmap(enemy_bullets[i]->tex);
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
        enemy[i]->pos.x = (itemCount * 20);
        enemy[i]->pos.y = (100) - enemy[i]->rect.w - (20 * rowCount);
        enemy[i]->startPos = enemy[i]->pos.x;
        enemy[i]->rowPosID = 20 * (11 - itemCount);
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

    const int moveSpeed = 1;

    int e;
    for (e = 0; e < MAX_ENEMIES; e++) if (enemy[e]) {
            if (enemy[e]->goLeft == 0)
            {
                enemy[e]->pos.x += moveSpeed * (Timestep);
            }

            if (enemy[e]->goLeft == 1)
            {
                enemy[e]->pos.x -= moveSpeed * (Timestep);
            }

            if (enemy[e]->pos.x >= 320 - (enemy[e]->rect.w + enemy[e]->rowPosID) && enemy[e]->goLeft == 0)
            {
                enemy[e]->goLeft = 1;
            }

            if (enemy[e]->pos.x <= enemy[e]->startPos + enemy[e]->rect.w && enemy[e]->goLeft == 1)
            {
                enemy[e]->goLeft = 0;
            }

            enemy[e]->shootTimer += 1 * Timestep/100;

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
                play_sample(pusher_snd, 255, 0, 1000, 0);
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
                //blit(enemy[e]->tex, buffer, enemy[e]->rect.x, enemy[e]->rect.y, enemy[e]->pos.x, enemy[e]->pos.y, enemy[e]->rect.x, enemy[e]->rect.y); //128, 32
                //draw_sprite(buffer, enemy[e]->tex, enemy[e]->pos.x, enemy[e]->pos.y);
                masked_blit(enemy[e]->tex, buffer, enemy[e]->rect.x, enemy[e]->rect.y, enemy[e]->pos.x, enemy[e]->pos.y, enemy[e]->rect.w, enemy[e]->rect.h);
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
            bullets[i]->pos.y -= 2 * Timestep;

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
            //void blit(BITMAP *source, BITMAP *dest, int source_x, int source_y, int dest_x, int dest_y, int width, int height);
            blit(bullets[i]->tex, buffer, 0, 0, bullets[i]->pos.x, bullets[i]->pos.y, bullets[i]->hitbox.x, bullets[i]->hitbox.y);
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
    p_move = 320 / 2 - player.hitbox.w / 2;
    player.pos.y = (200 - 20) - player.hitbox.h / 2;
    player.hitbox.x = player.pos.x;
    player.hitbox.y = player.pos.y;
    player.alive = 1;
}

void input()
{
    //controls

    if (key[KEY_SPACE])
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
        addBullet(player.pos.x + (player.hitbox.w / 2) - 10, player.pos.y);
        play_sample(blaster_snd, 255, 0, 1000, 0);
        A_BUTTON_DOWN = 1;
    }


    if (key[KEY_LEFT]) {
        //printf("LEFT! \r\n");
        p_move --;
    }

    if (key[KEY_RIGHT]) {
        //printf("RIGHT! \r\n");
        p_move ++;
    }

//  }

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
    else if (player.pos.x >= 320 - player.hitbox.w)
    {
        player.pos.x = 320 - player.hitbox.w;
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
            explo[e]->currentFrame_ex += 3 * Timestep/10;
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
            //void blit(BITMAP *source, BITMAP *dest, int source_x, int source_y, int dest_x, int dest_y, int width, int height);
            masked_blit(explo[i]->tex, buffer, explo[i]->rect.x, explo[i]->rect.y, explo[i]->pos.x, explo[i]->pos.y, 32, 32);//512, 32);
            //clear_bitmap(explo[i]->tex);
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
                        play_sample(explode_snd, 255, 0, 1000, 0);
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
                addExplo(player.pos.x - 32 / 2, player.pos.y - 32 / 2);
                removeEnemyBullet(i);
                play_sample(explode_snd, 255, 0, 1000, 0);
                //printf("BOOM!\n");
                break;
            }
        }

}

int main(int argc, const char **argv) {

    // Get basic stuff initialized
    // Initializes the Allegro library.
    if (allegro_init() != 0) {
        return 1;
    }

    // Installs the Allegro interrupt handlers.
    install_keyboard();

    install_timer();
    LOCK_VARIABLE(ticks);
    LOCK_FUNCTION(Ticker);
    install_int(Ticker, 1); // the speed can be specified in milliseconds (this is once a second)
    //install_int_ex(Ticker, MSEC_TO_TIMER(1));

    /* INITIALIZE SOUND */
    if(install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, NULL)) {
        allegro_message("Sound Error: %s\n", allegro_error);
        return 1;
    }

    /* install a MIDI sound driver */
    if (install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, NULL) != 0) {
        allegro_message("Error initialising sound system\n%s\n", allegro_error);
        return 1;
    }

    set_volume(255, 255); //Max volume

    set_color_depth(32);

    // Switch to graphics mode, 320x200.
    if (set_gfx_mode(GFX_AUTODETECT, 320, 200, 0, 0) != 0) {
        set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
        allegro_message("Cannot set graphics mode:\r\n%s\r\n", allegro_error);
        return 1;
    }

    set_palette(desktop_palette);

    buffer = create_bitmap(SCREEN_W, SCREEN_H);

    preload_asset();

    // original res 640x480
    // DOS res 320x200
    struct rectangle std_res;
    std_res.w = SCREEN_W;
    std_res.h = SCREEN_H;

    initEnemies();
    initPlayer();

    play_midi(music, TRUE);


    /* Loop until the user closes the window */
    while (!key[KEY_ESC])
    {
        curTime = ticks/10;
        Timestep = curTime - lastdelta;

        gameSessionTime += 1 * Timestep/100;

        if (gameSessionTime >= 3.f)
        {
            start_game = 1;
        }

        if (start_game == 0) {
            blit(fmg_splash, buffer, 0, 0, 0, 0, std_res.w, std_res.h);
        }
        else
        {
            clear_bitmap(fmg_splash);
        }


        if (start_game == 1)
        {

            clear_to_color(buffer, makecol(0, 0, 0));

            updateExplo();
            updateBullet();
            updateEnemyBullet();
            updateEnemies();

            if (player.alive == 1)
                updatePlayer(); //player

            updateLogic();

            blit(space, buffer, 0, 0, 0, 0, std_res.w, std_res.h);
            //clear_bitmap(space);

            drawExplo();
            drawEnemies();
            drawBullet();
            drawEnemyBullet();

            sprintf(textBuffer, "SCORE: % 05d", score);
            textout_ex(buffer, font, textBuffer, SCREEN_W - 100, SCREEN_H - 190, makecol(255,255,255), -1);

            if(enemies_killed <= 0)
                textout_centre_ex(buffer, font, "YOU WIN!", SCREEN_W / 2, SCREEN_H / 2, makecol(255,255,255), -1);

            if (player.alive == 1) {
                masked_blit(player.tex, buffer, 0, 0, player.pos.x, player.pos.y, player_tex->w, player_tex->h);
                //clear_bitmap(player_tex);
            }
            else
            {
                textout_centre_ex(buffer, font, "GAME OVER!", SCREEN_W / 2, SCREEN_H / 2, makecol(255,255,255), -1);
            }

        }

        blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);

        //clean up
        /*
        clear_bitmap(fmg_splash);
        clear_bitmap(space);
        clear_bitmap(enemy_bullet_tex);
        clear_bitmap(enemy_tex);
        clear_bitmap(bullet_tex);

        clear_bitmap(explo_tex);
        */

        lastdelta = curTime;

    }

    destroy_bitmap(fmg_splash);

    return 0;
}

END_OF_MAIN()