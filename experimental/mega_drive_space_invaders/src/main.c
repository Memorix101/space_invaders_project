#include <genesis.h>
#include "sprites.h"
#include "gfx.h"
#include "sounds.h"

typedef struct
{
	int x;
	int y;
	int w;
	int h;
	int velx;
	int vely;
	int health;
	Sprite *sprite;
	char name[6];
} Entity;

void Entity_setPos(Entity *e, int x, int y)
{
	e->x = x;
	e->y = y;
	SPR_setPosition(e->sprite, e->x, e->y);
}

char textBuffer[64];
int cur_bullet = 0;
int cur_explo = 0;
int score = 0;
int goLeft = 0; // bool
int bullet_speed = 5;
int player_speed = 5;
int enemy_speed = 1;
int key_firePressed = 0; // bool
int game_timer = 0;
int game_start = 0; // bool

int main()
{
	// u16 palette[64]; //we mix all palettes together

	JOY_init();
	// JOY_setEventHandler( &myJoyHandler );

	// Make PAL resolution
	// VDP_setScreenWidth320();
	// VDP_setScreenHeight240();
	VDP_init();
	// VDP_setPlaneSize(64, 64, TRUE);
	VDP_setPlaneSize(320, 240, TRUE);

	// PAL_setPalette(PAL1, space_png.palette->data, CPU); //Load the palette of the image into PAL1
	// PAL_setPalette(PAL2, space_png.palette->data, DMA);
	// VDP_drawImageEx(BG_A, &fmg_png, TILE_ATTR_FULL(PAL1, TRUE, FALSE, FALSE, 1), 0, 0, TRUE, DMA);
	VDP_loadTileSet(fmg_png.tileset, 1, DMA); // Load the tileset
	VDP_setTileMapEx(BG_B, fmg_png.tilemap, TILE_ATTR_FULL(PAL1, TRUE, FALSE, FALSE, 1), 0, 0, 0, 0, 40, 30, DMA);

	SPR_init(); // Initialize the sprite engine

	Entity player = {320 / 2 - 24 / 2, 240 - 35, 24, 16, 0, 0, 1, NULL, "PLAYER"};
	player.sprite = SPR_addSprite(&player_png, player.x, player.y, TILE_ATTR(PAL1, TRUE, FALSE, FALSE));

// sounds
// We’re also starting at 64 because the first 64 indexes (0 - 63) are reserved for music by the sound driver,
// so sound effects have to start at 64. Keep that in mind!
#define SFX_BLASTER 64
#define SFX_EXLODE1 65
#define SFX_PUSHER 66
#define MS_BODEN 0

	XGM_setPCM(SFX_BLASTER, blaster_sfx, sizeof(blaster_sfx));
	XGM_setPCM(SFX_EXLODE1, explode1_sfx, sizeof(explode1_sfx));
	XGM_setPCM(SFX_PUSHER, pusher_sfx, sizeof(pusher_sfx));
	//XGM_setPCM(MS_BODEN, boden_sfx, sizeof(boden_sfx));

	//XGM_startPlayPCM(MS_BODEN, 1, SOUND_PCM_CH1);
	XGM_setLoopNumber(-1);
	XGM_startPlay(xgm_music);

#define MAX_ENEMIES 13
	Entity enemies[MAX_ENEMIES];
	Entity *enemy = enemies; //{64, 0, 24, 24, 0, 0, 0, NULL, "ENEMY" };
	int rowCount = 0;
	int itemCount = 0;
	for (int i = 0; i < MAX_ENEMIES; i++)
	{
		if (i % 6 == 0)
		{
			itemCount = 0;
			rowCount++;
		}
		itemCount++;

		enemy->x = -32;
		enemy->y = -32;
		enemy->w = 24;
		enemy->h = 24;
		enemy->velx = (random() % (20 - 5 + 1)) + 5; // (max - min + 1) + min
		enemy->vely = 0;							 // timer
		enemy->health = 1;
		enemy->sprite = SPR_addSprite(&enemy_png, enemy->x, enemy->y, TILE_ATTR(PAL1, TRUE, FALSE, FALSE));
		sprintf(enemy->name, "Enemy%d", i);

		if (i != 0) // workaround
		{
			enemy->x = 64 + itemCount * 32;
			enemy->y = 24 + (rowCount * 32) - 32;
		}

		enemy++;
	}

	// workaround
	enemies[12].x = 64 + 32;
	enemies[12].y = 24;

#define MAX_BULLETS 5
	Entity bullets[MAX_BULLETS];
	Entity *bullet = bullets;
	for (int i = 0; i < MAX_BULLETS; i++)
	{
		bullet->x = -32;
		bullet->y = -32;
		bullet->w = 8;
		bullet->h = 32;
		bullet->health = 0;
		bullet->sprite = SPR_addSprite(&bullet_png, bullet->x, bullet->y, TILE_ATTR(PAL1, TRUE, FALSE, FALSE));
		sprintf(bullet->name, "Bullet%d", i);
		SPR_setAnim(bullet->sprite, 0);
		bullet++;
	}

#define MAX_EXPLO 6
	Entity explos[MAX_EXPLO];
	Entity *explo = explos;
	for (int i = 0; i < MAX_EXPLO; i++)
	{
		explo->x = 32 * i;
		explo->y = -32;
		explo->w = 18;
		explo->h = 16;
		explo->health = 1;
		explo->sprite = SPR_addSprite(&explo_png, explo->x, explo->y, TILE_ATTR(PAL1, TRUE, FALSE, FALSE));
		SPR_setAnim(explo->sprite, 0);
		SPR_setDepth(explo->sprite, 5);
		sprintf(explo->name, "Explo%d", i);
		explo++;
	}

	while (1)
	{
		// VDP_clearPlan(PLAN_A, TRUE);
		// VDP_setPlaneSize(64, 32, TRUE);
		/*Clear the screen*/
		/*VDP_clearPlane(BG_A, 1);
		VDP_clearPlane(BG_B, 1);
		VDP_clearPlane(WINDOW, 1);*/

		game_timer++;

		if (game_timer <= 200)
		{
			// VDP_setPlaneSize(320, 240, TRUE);
			// VDP_drawImageEx(BG_A, &fmg_png, TILE_ATTR_FULL(PAL1, TRUE, FALSE, FALSE, 1), 0, 0, TRUE, DMA);
			SPR_setVisibility(player.sprite, HIDDEN);
		}
		else
		{
			if (game_start == 0)
			{
				// BMP_clear();
				// VDP_clearPlane(BG_A, TRUE);
				VDP_setTextPlane(BG_A);
				// VDP_setPlaneSize(320, 240, TRUE);
				VDP_loadTileSet(space_png.tileset, 1, DMA); // Load the tileset
				VDP_setTileMapEx(BG_B, space_png.tilemap, TILE_ATTR_FULL(PAL1, TRUE, FALSE, FALSE, 1), 0, 0, 0, 0, 40, 30, DMA);
				game_start = 1;
				SPR_setVisibility(player.sprite, VISIBLE);
			}

			Entity_setPos(&player, player.x, player.y);

			for (int i = 0; i < MAX_ENEMIES; i++)
			{
				if (enemies[i].health == 0)
				{
					SPR_setVisibility(enemies[i].sprite, HIDDEN);
					// continue;
				}
				else
				{
					enemies[i].vely++;
					SPR_setVisibility(enemies[i].sprite, VISIBLE);

					// vely = timer ; velx = timer_limit
					if (enemies[i].vely >= enemies[i].velx)
					{
						// enemies[i].health = 0;
						enemies[i].vely = 0;
					}
				}

				if (enemies[6].x <= 0 && goLeft == 1)
				{
					goLeft = 0;
					// enemies[6].x = enemies[0].x; // workaround - enemy pos drifting
					enemy_speed = enemy_speed * (-1);
				}
				else if (enemies[6].x >= 136 && goLeft == 0)
				{
					goLeft = 1;
					enemy_speed = enemy_speed * (-1);
				}

				enemies[i].x += enemy_speed;
				Entity_setPos(&enemies[i], enemies[i].x, enemies[i].y);
			}

			// collision
			for (int i = 0; i < MAX_BULLETS; i++)
			{
				for (int e = 0; e < MAX_ENEMIES; e++)
				{
					if (enemies[e].health == 1 && bullets[i].health == 1)
					{

						if ((bullets[i].x < enemies[e].x + enemies[e].w &&
							 bullets[i].x + bullets[i].w > enemies[e].x &&
							 bullets[i].y < enemies[e].y + enemies[e].h &&
							 bullets[i].h + bullets[i].y > enemies[e].y))
						{
							bullets[i].health = 0;
							Entity_setPos(&bullets[i], -32, -32);
							enemies[e].health = 0;
							score += 100;

							// explosion
							explos[cur_explo].health = 1;
							cur_explo++;

							if (cur_explo >= MAX_EXPLO - 1)
							{
								cur_explo = 0;
							}

							Entity_setPos(&explos[cur_explo], enemies[e].x + 4, enemies[e].y + 8);
							SPR_setAnim(explos[cur_explo].sprite, 0);
							SPR_setFrame(explos[cur_explo].sprite, 0);
							XGM_startPlayPCM(SFX_EXLODE1, 15, SOUND_PCM_CH1);
						}
					}
				}
			}

			for (int i = 0; i < MAX_EXPLO; i++)
			{
				if (explos[i].health == 0)
				{
					SPR_setVisibility(explos[i].sprite, HIDDEN);
					continue;
				}
				else if (explos[i].health == 1)
				{
					SPR_setVisibility(explos[i].sprite, VISIBLE);
				}

				if (explos[i].sprite->frameInd >= 14)
				{
					Entity_setPos(&explos[i], -32, -32);
				}
			}

			for (int i = 0; i < MAX_BULLETS; i++)
			{
				if (bullets[i].health == 0)
				{
					SPR_setVisibility(bullets[i].sprite, HIDDEN);
					continue;
				}
				else if (bullets[i].health == 1)
				{
					SPR_setVisibility(bullets[i].sprite, VISIBLE);
					bullets[i].y -= bullet_speed;
					Entity_setPos(&bullets[i], bullets[i].x, bullets[i].y);
				}

				if (bullets[i].y <= -16)
				{
					bullets[i].health = 0;
				}
			}

			sprintf(textBuffer, "SCORE: % 05d", score);
			VDP_drawTextBG(BG_A, textBuffer, 26, 1);

			u16 state = JOY_readJoypad(JOY_1);
			if (state & BUTTON_START) // just pressed
			{
				// player 1 press START button
				// reset
			}

			if (state & (BUTTON_A | BUTTON_B | BUTTON_C)) // is down
			{
				if (key_firePressed == 0)
				{
					key_firePressed = 1;

					// shoot
					bullets[cur_bullet].health = 1;
					bullets[cur_bullet].x = player.x + 6;
					bullets[cur_bullet].y = player.y;
					cur_bullet++;

					if (cur_bullet >= MAX_BULLETS - 1)
					{
						cur_bullet = 0;
					}

					XGM_startPlayPCM(SFX_BLASTER, 15, SOUND_PCM_CH1);
				}
			}
			else
			{
				key_firePressed = 0;
			}

			if (state & (BUTTON_RIGHT)) // is down
			{
				if (player.x < 320 - player.w)
				{
					player.x += player_speed;
				}
			}

			if (state & (BUTTON_LEFT)) // is down
			{
				if (player.x > 0)
				{
					player.x -= player_speed;
				}
			}
		}

		SPR_update();

		PAL_setPalette(PAL1, player_png.palette->data, CPU);

		SYS_doVBlankProcess();
	}
	return (0);
}