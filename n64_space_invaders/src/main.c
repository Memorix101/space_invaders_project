#include <libdragon.h>
#include <stdlib.h>
#include <string.h>

#define VIRT_W      640
#define VIRT_H      480
#define SCREEN_W    640
#define SCREEN_H    480
#define DRAW_SCALE  1.0f

#define MAX_BULLETS         50
#define MAX_ENEMY_BULLETS  100
#define MAX_ENEMIES         40
#define MAX_EXPLO          100

#define ENEMY_FRAME_W   32
#define ENEMY_FRAME_H   32
#define ENEMY_FRAMES     4
/* Footprint of an explosion in virtual (640x480) space. This stays at 128
 * regardless of how big the texture actually is, so the explosion keeps its
 * original on-screen size and centering when the sheet gets resized. */
#define EXPLO_VIRT_W   128
#define EXPLO_VIRT_H   128
#define EXPLO_FRAMES    16

#define BULLET_SPEED   350.0f
#define PLAYER_SPEED   250.0f
#define ENEMY_SPEED    150.0f

/* Mixer channel allocation. All waves are mono, so each sound takes exactly
 * one channel. A stereo wave would occupy two consecutive channels. */
#define CHANNEL_SFX_EXPLO    0
#define CHANNEL_SFX_BLASTER  1
#define CHANNEL_SFX_PUSHER   2
#define CHANNEL_MUSIC        3

#define FONT_VERMIN  1

typedef struct {
    float x, y;
    int   w, h;
    bool  active;
} bullet_t;

typedef struct {
    float x, y;
    float startPos;
    int   rowPosID;
    int   goLeft;
    bool  alive;
    bool  active;
    float shootTimer;
    float shootTimeLimit;
    float speed;
} enemy_t;

typedef struct {
    float x, y;
    float currentFrame;
    bool  active;
} explo_t;

typedef struct {
    float x, y;
    int   w, h;
    float speed;
    bool  alive;
} player_t;

/* --- Assets --------------------------------------------------------------- */
static sprite_t *spr_space3, *spr_enemy, *spr_player, *spr_bullet;
static sprite_t *spr_enemy_bullet, *spr_explo, *spr_gameover, *spr_win;

/* Size of a single explosion frame inside the sheet, derived from the loaded
 * sprite so that resizing explode.png needs no code change. */
static int explo_frame_w, explo_frame_h;

/* Pixel surface of the explosion sheet. RDP texture coordinates are 10 bit,
 * so anything at or beyond 1024 cannot be reached through the s0 field of
 * rdpq_blitparms_t. The sheet is wider than that, so each frame is addressed
 * as a sub-surface instead, which re-bases the pointer and keeps the
 * coordinates small. Note this requires explode.sprite to stay RGBA16:
 * rdpq_tex_blit does not upload a palette, unlike rdpq_sprite_blit. */
static surface_t explo_sheet;

static rdpq_font_t *font_vermin;
static wav64_t snd_blaster, snd_pusher, snd_explo, music;

/* --- Game state ----------------------------------------------------------- */
static player_t player;
static enemy_t  enemy[MAX_ENEMIES];
static bullet_t bullets[MAX_BULLETS];
static bullet_t enemy_bullets[MAX_ENEMY_BULLETS];
static explo_t  explo[MAX_EXPLO];

static int   enemies_killed = MAX_ENEMIES;
static float currentFrame   = 0.0f;
static int   rowCount       = 0;
static int   itemCount      = 0;
static int   score          = 0;
static int   gameover       = 0;
static float deltaTime      = 0.0f;

/* --- Drawing helpers ------------------------------------------------------ */

/* Blit a sub-rect of a sprite at a virtual (640x480) position, stretched to
 * cover vw x vh in virtual space. This decouples the texture resolution from
 * the size the sprite occupies in the game world. */
static void blit_virt_scaled(sprite_t *spr, float vx, float vy,
                             int s0, int sw, int sh, float vw, float vh)
{
    rdpq_sprite_blit(spr, vx * DRAW_SCALE, vy * DRAW_SCALE, &(rdpq_blitparms_t){
        .s0      = s0,
        .width   = sw,
        .height  = sh,
        .scale_x = (vw * DRAW_SCALE) / sw,
        .scale_y = (vh * DRAW_SCALE) / sh,
    });
}

/* Blit a sprite, or a sub-rect of it, at a virtual (640x480) position, using
 * the source pixels 1:1 as virtual units. */
static void blit_virt(sprite_t *spr, float vx, float vy, int s0, int w, int h)
{
    blit_virt_scaled(spr, vx, vy, s0, w, h, w, h);
}

/* Stretch a full-screen image to the framebuffer regardless of its source
 * size. space3 is 800x600, the UI screens are 640x480. */
static void blit_fullscreen(sprite_t *spr)
{
    /*rdpq_sprite_blit(spr, 0, 0, &(rdpq_blitparms_t){
        .scale_x = (float)SCREEN_W / spr->width,
        .scale_y = (float)SCREEN_H / spr->height,
    });*/

    rdpq_sprite_blit(spr, 0, 0, NULL);
}

/* AABB test with the same semantics as SDL_HasIntersection: edges touching
 * does not count as an intersection. */
static bool rects_overlap(float ax, float ay, int aw, int ah,
                          float bx, float by, int bw, int bh)
{
    return ax < bx + bw && ax + aw > bx &&
           ay < by + bh && ay + ah > by;
}

static void load_assets(void)
{
    spr_space3       = sprite_load("rom:/space3.sprite");
    spr_enemy        = sprite_load("rom:/invader32x32x4.sprite");
    spr_player       = sprite_load("rom:/player.sprite");
    spr_bullet       = sprite_load("rom:/bullet.sprite");
    spr_enemy_bullet = sprite_load("rom:/enemy-bullet.sprite");
    spr_explo        = sprite_load("rom:/explode.sprite");
    spr_gameover     = sprite_load("rom:/gameover_ui.sprite");
    spr_win          = sprite_load("rom:/win_ui.sprite");

    /* The sheet holds EXPLO_FRAMES frames side by side, so a frame is just
     * the sheet width divided by the frame count. */
    explo_frame_w = spr_explo->width / EXPLO_FRAMES;
    explo_frame_h = spr_explo->height;
    explo_sheet   = sprite_get_pixels(spr_explo);
}

/* --- Enemy bullets -------------------------------------------------------- */

static void addEnemyBullet(float x, float y)
{
    for (int i = 0; i < MAX_ENEMY_BULLETS; i++) {
        if (!enemy_bullets[i].active) {
            enemy_bullets[i] = (bullet_t){
                .x = x, .y = y,
                .w = spr_enemy_bullet->width,
                .h = spr_enemy_bullet->height,
                .active = true,
            };
            return;
        }
    }
}

static void removeEnemyBullet(int i)
{
    enemy_bullets[i].active = false;
}

static void updateEnemyBullet(void)
{
    for (int i = 0; i < MAX_ENEMY_BULLETS; i++) if (enemy_bullets[i].active) {
        enemy_bullets[i].y += BULLET_SPEED * deltaTime;

        if (enemy_bullets[i].y >= VIRT_H - 9) {
            removeEnemyBullet(i);
        }
    }
}

static void drawEnemyBullet(void)
{
    for (int i = 0; i < MAX_ENEMY_BULLETS; i++) if (enemy_bullets[i].active) {
        blit_virt(spr_enemy_bullet, enemy_bullets[i].x, enemy_bullets[i].y,
                  0, spr_enemy_bullet->width, spr_enemy_bullet->height);
    }
}

/* --- Enemies -------------------------------------------------------------- */

static void initEnemies(void)
{
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (i % 10 == 0) {
            itemCount = 0;
            rowCount++;
        }
        itemCount++;

        enemy[i] = (enemy_t){
            .x        = itemCount * 40,
            .y        = 40 * rowCount,
            .startPos = itemCount * 40,
            /* Staggered turnaround point per column, this is what produces
             * the wave shaped formation movement. */
            .rowPosID = 40 * (11 - itemCount),
            .speed    = ENEMY_SPEED,
            .goLeft   = 0,
            .alive    = true,
            .active   = true,
            .shootTimer     = 0.0f,
            .shootTimeLimit = (float)((rand() % (20 - 3)) + 3),
        };
    }
}

static void animatorEnemies(void)
{
    currentFrame += 15.0f * deltaTime;

    if ((int)currentFrame >= ENEMY_FRAMES) {
        currentFrame = 0.0f;
    }
}

static void removeEnemy(int i)
{
    enemy[i].active = false;
}

static void updateEnemies(void)
{
    animatorEnemies();

    for (int e = 0; e < MAX_ENEMIES; e++) if (enemy[e].active) {
        if (enemy[e].goLeft == 0) {
            enemy[e].x += enemy[e].speed * deltaTime;
        } else {
            enemy[e].x -= enemy[e].speed * deltaTime;
        }

        if (enemy[e].x >= VIRT_W - (ENEMY_FRAME_W + enemy[e].rowPosID) && enemy[e].goLeft == 0) {
            enemy[e].goLeft = 1;
        }

        if (enemy[e].x <= enemy[e].startPos + ENEMY_FRAME_W && enemy[e].goLeft == 1) {
            enemy[e].goLeft = 0;
        }

        enemy[e].shootTimer += deltaTime;

        if (enemy[e].shootTimer >= enemy[e].shootTimeLimit) {
            enemy[e].shootTimer = 0.0f;

            if (enemy[e].alive) {
                addEnemyBullet(enemy[e].x + ENEMY_FRAME_W / 2 - 4, enemy[e].y - 4);
                wav64_play(&snd_pusher, CHANNEL_SFX_PUSHER);
            }
        }
    }
}

static void drawEnemies(void)
{
    int frame = (int)currentFrame;

    for (int e = 0; e < MAX_ENEMIES; e++) if (enemy[e].active && enemy[e].alive) {
        blit_virt(spr_enemy, enemy[e].x, enemy[e].y,
                  frame * ENEMY_FRAME_W, ENEMY_FRAME_W, ENEMY_FRAME_H);
    }
}

/* --- Player bullets ------------------------------------------------------- */

static void addBullet(float x, float y)
{
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!bullets[i].active) {
            bullets[i] = (bullet_t){
                .x = x, .y = y,
                .w = spr_bullet->width,
                .h = spr_bullet->height,
                .active = true,
            };
            return;
        }
    }
}

static void removeBullet(int i)
{
    bullets[i].active = false;
}

static void updateBullet(void)
{
    for (int i = 0; i < MAX_BULLETS; i++) if (bullets[i].active) {
        bullets[i].y -= BULLET_SPEED * deltaTime;

        if (bullets[i].y <= 0) {
            removeBullet(i);
        }
    }
}

static void drawBullet(void)
{
    for (int i = 0; i < MAX_BULLETS; i++) if (bullets[i].active) {
        blit_virt(spr_bullet, bullets[i].x, bullets[i].y,
                  0, spr_bullet->width, spr_bullet->height);
    }
}

/* --- Player --------------------------------------------------------------- */

static void initPlayer(void)
{
    player.w     = spr_player->width;
    player.h     = spr_player->height;
    player.speed = PLAYER_SPEED;
    player.x     = VIRT_W / 2 - player.w / 2;
    player.y     = (VIRT_H - 60) - player.h / 2;
    player.alive = true;
}

static void input(void)
{
    joypad_buttons_t held = joypad_get_buttons_held(JOYPAD_PORT_1);
    joypad_inputs_t  in   = joypad_get_inputs(JOYPAD_PORT_1);

    /* Stick reacts from half deflection, matching the 0.5 threshold the SDL
     * version used for the gamepad axis. */
    bool move_right = held.d_right || in.stick_x >=  64;
    bool move_left  = held.d_left  || in.stick_x <= -64;

    if (move_right) {
        player.x += player.speed * deltaTime;
    } else if (move_left) {
        player.x -= player.speed * deltaTime;
    }
}

static void fire(void)
{
    addBullet(player.x + player.w / 2 - 3, player.y);
    wav64_play(&snd_blaster, CHANNEL_SFX_BLASTER);
}

static void updatePlayer(void)
{
    input();

    if (player.x <= 0) {
        player.x = 0;
    } else if (player.x >= VIRT_W - player.w) {
        player.x = VIRT_W - player.w;
    }
}

/* --- Explosions ----------------------------------------------------------- */

static void addExplo(float x, float y)
{
    for (int i = 0; i < MAX_EXPLO; i++) {
        if (!explo[i].active) {
            explo[i] = (explo_t){
                .x = x, .y = y,
                .currentFrame = 0.0f,
                .active = true,
            };
            return;
        }
    }
}

static void updateExplo(void)
{
    for (int e = 0; e < MAX_EXPLO; e++) if (explo[e].active) {
        explo[e].currentFrame += 30.0f * deltaTime;

        if ((int)explo[e].currentFrame >= EXPLO_FRAMES) {
            explo[e].active = false;
        }
    }
}

static void drawExplo(void)
{
    /* rdpq_tex_blit works on a plain surface and, unlike rdpq_sprite_blit,
     * does not touch the TLUT mode. The CI8 background left palette lookup
     * enabled, which would run these RGBA16 pixels through the palette and
     * wreck the colors, so switch it off explicitly. */
    rdpq_mode_tlut(TLUT_NONE);

    for (int i = 0; i < MAX_EXPLO; i++) if (explo[i].active) {
        int f = (int)explo[i].currentFrame;
        if (f >= EXPLO_FRAMES) f = EXPLO_FRAMES - 1;

        surface_t frame = surface_make_sub(&explo_sheet, f * explo_frame_w, 0,
                                           explo_frame_w, explo_frame_h);

        rdpq_tex_blit(&frame, explo[i].x * DRAW_SCALE, explo[i].y * DRAW_SCALE,
                      &(rdpq_blitparms_t){
                          .scale_x = (EXPLO_VIRT_W * DRAW_SCALE) / explo_frame_w,
                          .scale_y = (EXPLO_VIRT_H * DRAW_SCALE) / explo_frame_h,
                      });
    }
}

/* --- Collisions and scoring ----------------------------------------------- */

static void updateLogic(void)
{
    for (int i = 0; i < MAX_BULLETS; i++) if (bullets[i].active) {
        for (int e = 0; e < MAX_ENEMIES; e++) if (enemy[e].active && enemy[e].alive) {
            if (rects_overlap(bullets[i].x, bullets[i].y, bullets[i].w, bullets[i].h,
                              enemy[e].x, enemy[e].y, ENEMY_FRAME_W, ENEMY_FRAME_H)) {
                enemy[e].alive = false;
                addExplo(bullets[i].x - EXPLO_VIRT_W / 2, bullets[i].y - EXPLO_VIRT_H / 2);
                removeBullet(i);
                wav64_play(&snd_explo, CHANNEL_SFX_EXPLO);
                score += 100;
                enemies_killed--;
                break;
            }
        }
    }

    for (int e = 0; e < MAX_ENEMIES; e++) if (enemy[e].active && !enemy[e].alive) {
        removeEnemy(e);
    }

    for (int b = 0; b < MAX_ENEMY_BULLETS; b++) if (enemy_bullets[b].active) {
        if (player.alive &&
            rects_overlap(enemy_bullets[b].x, enemy_bullets[b].y,
                          enemy_bullets[b].w, enemy_bullets[b].h,
                          player.x, player.y, player.w, player.h)) {
            player.alive = false;
            addExplo(player.x - EXPLO_VIRT_W / 2, player.y - EXPLO_VIRT_H / 2);
            removeEnemyBullet(b);
            wav64_play(&snd_explo, CHANNEL_SFX_EXPLO);
            /* Replaces SDL_HapticRumblePlay, works if a Rumble Pak is present */
            joypad_set_rumble_active(JOYPAD_PORT_1, true);
            break;
        }
    }
}

static void reset(void)
{
    enemies_killed = MAX_ENEMIES;
    gameover       = 0;
    score          = 0;
    currentFrame   = 0.0f;
    rowCount       = 0;
    itemCount      = 0;

    memset(enemy,         0, sizeof(enemy));
    memset(bullets,       0, sizeof(bullets));
    memset(enemy_bullets, 0, sizeof(enemy_bullets));
    memset(explo,         0, sizeof(explo));

    initEnemies();
    player.x     = VIRT_W / 2 - player.w / 2;
    player.alive = true;

    joypad_set_rumble_active(JOYPAD_PORT_1, false);
}

/* --- Splash screen -------------------------------------------------------- */

static void show_splash(void)
{
    sprite_t *spr_splash = sprite_load("rom:/fmg_splash.sprite");
    uint32_t start = TICKS_READ();

    /* Hold for two seconds. The mixer has to keep running or the music
     * stutters, which is why this is a loop and not a plain wait. */
    while (TICKS_DISTANCE(start, TICKS_READ()) < (int32_t)(TICKS_PER_SECOND * 2)) {
        surface_t *disp = display_get();
        rdpq_attach(disp, NULL);
        rdpq_set_mode_standard();
        blit_fullscreen(spr_splash);
        rdpq_detach_show();

        if (audio_can_write()) {
            short *buf = audio_write_begin();
            mixer_poll(buf, audio_get_buffer_length());
            audio_write_end();
        }
    }

    /* 640x480 as CI8 is roughly 300 KB and is never needed again */
    sprite_free(spr_splash);
}

int main(void)
{
    debug_init_usblog();
    debug_init_emulog();

    /* The artwork is authored for 640x480, so render there and blit it 1:1.
     * The N64 can only do this interlaced, which flickers on a real CRT.
     * Two buffers instead of three, since each one is now 600 KB.
     * FILTERS_RESAMPLE would run a bilinear resize pass over the whole output
     * and softens every sprite. The art is pixel work, so keep it sharp. */
    display_init(RESOLUTION_640x480, DEPTH_16_BPP, 2, GAMMA_NONE, FILTERS_DISABLED); // DEPTH_32_BPP requires expansion pack 
    rdpq_init();
    joypad_init();
    timer_init();

    int ret = dfs_init(DFS_DEFAULT_LOCATION);
    assert(ret == DFS_ESUCCESS);

    audio_init(48000, 4);
    mixer_init(16);
    wav64_init_compression(3);   /* the music track is Opus compressed */

    /* The music streams at a higher rate than the SFX, so it gets its own
     * frequency limit on its channel. */
    mixer_ch_set_limits(CHANNEL_MUSIC, 0, 128000, 0);

    load_assets();

    font_vermin = rdpq_font_load("rom:/vermin_vibes_1989.font64");
    rdpq_text_register_font(FONT_VERMIN, font_vermin);

    wav64_open(&snd_blaster, "rom:/blaster.wav64");
    wav64_open(&snd_pusher,  "rom:/pusher.wav64");
    wav64_open(&snd_explo,   "rom:/explode1.wav64");
    wav64_open(&music,       "rom:/bodenstaendig.wav64");
    wav64_set_loop(&music, true);
    wav64_play(&music, CHANNEL_MUSIC);

    show_splash();

    srand(TICKS_READ());
    initEnemies();
    initPlayer();

    heap_stats_t heap;
    sys_get_heap_stats(&heap);
    debugf("RAM: %d bytes, heap used: %d, free: %d\n",
           get_memory_size(), heap.used, heap.total - heap.used);

    uint32_t last_ticks = TICKS_READ();

    while (1)
    {
        uint32_t now = TICKS_READ();
        deltaTime = (float)TICKS_DISTANCE(last_ticks, now) / (float)TICKS_PER_SECOND;
        last_ticks = now;

        /* Do not let the first frame or a hitch make the physics jump */
        if (deltaTime <= 0.0f || deltaTime > 0.1f) {
            deltaTime = 1.0f / 60.0f;
        }

        joypad_poll();
        joypad_buttons_t pressed = joypad_get_buttons_pressed(JOYPAD_PORT_1);

        if (pressed.a && player.alive && !gameover) {
            fire();
        }

        if (pressed.start && gameover) {
            reset();
        }

        updateExplo();
        updateBullet();
        updateEnemyBullet();
        updateEnemies();

        if (player.alive) {
            updatePlayer();
        }

        updateLogic();

        /* --- Rendering --- */
        surface_t *disp = display_get();
        rdpq_attach(disp, NULL);
        rdpq_set_mode_standard();

        blit_fullscreen(spr_space3);

        /* Everything from here on blends its alpha over the background */
        rdpq_mode_blender(RDPQ_BLENDER_MULTIPLY);

        drawExplo();
        drawEnemies();
        drawBullet();
        drawEnemyBullet();

        if (player.alive) {
            blit_virt(spr_player, player.x, player.y, 0, player.w, player.h);
        } else {
            blit_fullscreen(spr_gameover);
            gameover = 1;
        }

        if (enemies_killed <= 0) {
            blit_fullscreen(spr_win);
            gameover = 1;
        }

        rdpq_text_printf(&(rdpq_textparms_t){
                             .width = SCREEN_W - 20,
                             .align = ALIGN_RIGHT,
                         },
                         FONT_VERMIN, 10, 24, "SCORE: %05d", score);

        rdpq_detach_show();

        /* Refill the audio buffer as soon as one is free */
        if (audio_can_write()) {
            short *buf = audio_write_begin();
            mixer_poll(buf, audio_get_buffer_length());
            audio_write_end();
        }
    }
}
