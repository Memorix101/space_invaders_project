  #include <kos.h>
  #include <stdio.h>
  #include <SDL/SDL.h>
  #include <SDL\SDL_config_dreamcast.h>
  #include <mp3/sndserver.h>
  #include "SDL_image.h"
  #include <math.h>
  #include <png/png.h>
  #include <zlib/zlib.h>
  #include <dc/fmath.h>

  /* You can safely remove this line if you don't use a ROMDISK */
  extern uint8 romdisk[];
  KOS_INIT_FLAGS(INIT_DEFAULT | INIT_MALLOCSTATS);
  KOS_INIT_ROMDISK(romdisk);

  semaphore_t sem;

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
  };

  //base SDL stuff
  SDL_Event e;
  SDL_Surface* space = NULL;
  SDL_Surface* screen = NULL;
  SDL_Joystick* joystick;

  //player
  #define MAX_BULLETS 50
  struct player_t player;
  int p_move;

  //enemy
  #define MAX_ENEMY_BULLETS 100
  #define MAX_ENEMIES 40
  struct enemy_t *enemy[MAX_ENEMIES] = { NULL };
  int currentFrame = 0;

  //stuff
  struct bullet_t *bullets[MAX_BULLETS] = { NULL };
  struct enemy_bullet_t *enemy_bullets[MAX_ENEMY_BULLETS] = { NULL };
  #define MAX_EXPLO 100
  struct explo_t *explo[MAX_EXPLO] = { NULL };
  int rowCount = 0;
  int itemCount = 0;
  int DeltaTime;
  int lastTick;

  float c;

  kthread_t *snd_music, *snd_blaster, *snd_pusher, *snd_explode;

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
  		enemy_bullets[i]->tex = IMG_Load("rd/enemy-bullet.png");
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
      SDL_FreeSurface(enemy_bullets[i]->tex);
  		free(enemy_bullets[i]);
  		enemy_bullets[i] = NULL;
  	}
  }
  void updateEnemyBullet()
  {
  	int i;
  	for (i = 0; i < MAX_ENEMY_BULLETS; i++) if (enemy_bullets[i])
  	{
  		enemy_bullets[i]->pos.y += 15;

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
  		enemy[i] = malloc(sizeof *enemy[i]); //visual studio needs that "stupid" cast to operate >_>
  		enemy[i]->tex = IMG_Load("rd/invader32x32x4.png"); //invader32x32x4
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
  		enemy[i]->hitbox.w = enemy[i]->rect.w;
  		enemy[i]->hitbox.h = enemy[i]->rect.h;
  		enemy[i]->hitbox.x = enemy[i]->pos.x;
  		enemy[i]->hitbox.y = enemy[i]->pos.y;
  		enemy[i]->shoot = 0;
  		enemy[i]->shootTimer = 0;
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
  	currentFrame += 1;

  	if (currentFrame >= 4)
  	{
  		currentFrame = 0;
  	}

  	int e;
  	for (e = 0; e < MAX_ENEMIES; e++) if (enemy[e]) {
  		enemy[e]->rect.x = currentFrame * 32;
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
  			enemy[e]->pos.x += moveSpeed;
  		}

  		if (enemy[e]->goLeft == 1)
  		{
  			enemy[e]->pos.x -= moveSpeed;
  		}

  		if (enemy[e]->pos.x >= 640 - (enemy[e]->rect.w + enemy[e]->rowPosID) && enemy[e]->goLeft == 0)
  		{
  			enemy[e]->goLeft = 1;
  		}

  		if (enemy[e]->pos.x <= enemy[e]->startPos + enemy[e]->rect.w && enemy[e]->goLeft == 1)
  		{
  			enemy[e]->goLeft = 0;
  		}

  		enemy[e]->shootTimer += 1 * DeltaTime;
  				
  		//c = enemy[1]->shootTimer += 1 * DeltaTime;;
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
  			addEnemyBullet(enemy[e]->pos.x + enemy[e]->rect.w/2 - 4, enemy[e]->pos.y - 4);
        sample2_channel = snd_sfx_play(sample2, 255, 128);
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
  		bullets[i]->tex = IMG_Load("rd/bullet.png");
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
      SDL_FreeSurface(bullets[i]->tex);
  		free(bullets[i]);
  		bullets[i] = NULL;
  	}
  }

  void updateBullet()
  {
  	int i;
  	for (i = 0; i < MAX_BULLETS; i++) if (bullets[i])
  	{
  		bullets[i]->pos.y -= 15;

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
    player.tex = IMG_Load("rd/player.png");
    p_move = 640 / 2 - player.tex->w / 2;
    player.pos.y = (480 - 60) - player.tex->h / 2;
    player.hitbox.w = player.tex->w;
    player.hitbox.h = player.tex->h;
    player.hitbox.x = player.pos.x;
    player.hitbox.y = player.pos.y;
    player.alive = 1;
  }

  void input()
  {
        //dc controls
        cont = maple_enum_type(0, MAPLE_FUNC_CONTROLLER);

        if(cont) {
            state = (cont_state_t *)maple_dev_status(cont);

          if(state->buttons & CONT_A) {
                //printf("SHOOT! \r\n");
                addBullet(player.pos.x + player.tex->w / 2 - 3, player.pos.y);
                sample1_channel = snd_sfx_play(sample1, 255, 128);
              }            

                if(state->buttons & CONT_DPAD_LEFT || state->joyx <= -1) {
                   //printf("LEFT! \r\n");
                   p_move -= 15;
              }         

              if(state->buttons & CONT_DPAD_RIGHT || state->joyx >= 1) {
                  //printf("RIGHT! \r\n");
                  p_move += 15;
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
    else if (player.pos.x >= 640 - player.tex->w)
    {
      player.pos.x = 640 - player.tex->w;
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
  		explo[i]->tex = IMG_Load("rd/explode.png");
  		explo[i]->pos.x = x;
  		explo[i]->pos.y = y;
  		explo[i]->rect.w = 128;
  		explo[i]->rect.h = 129;
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
  		explo[e]->rect.x = explo[e]->currentFrame_ex * 128;

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
      SDL_FreeSurface(explo[i]->tex);
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
  			removeExplo(explo[i]);
  		}
  	}
  }

  void drawExplo()
  {
  	int i;
  	for (i = 0; i < MAX_EXPLO; i++) if (explo[i])
  	{
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
  				addExplo(bullets[i]->pos.x - 128 / 2, bullets[i]->pos.y - 128 / 2);
  				removeBullet(i);        
          sample3_channel = snd_sfx_play(sample3, 255, 128);
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

  	//Start SDL
  	SDL_Init(SDL_INIT_VIDEO);
  	SDL_ShowCursor(SDL_DISABLE);
  
    snd_stream_init();
    mp3_init();


  	//Set up screen
  	Uint32 flags = SDL_VIDEO_DRIVER_DC | SDL_SWSURFACE | SDL_DOUBLEBUF | SDL_FULLSCREEN;// | SDL_INIT_JOYSTICK; // | SDL_FULLSCREEN;
  	screen = SDL_SetVideoMode(640, 480, 16, flags);

  	SDL_Surface * fmg_splash = IMG_Load("rd/fmg_splash.png");
  	SDL_BlitSurface(fmg_splash, NULL, screen, NULL);
  	SDL_Flip(screen);
  	SDL_Delay(2000);
  	SDL_FreeSurface(fmg_splash);

  	//Load image
  	space = IMG_Load("rd/space3.png");

  	initEnemies();
  	initPlayer();

  snd_init();
  sample1 = snd_sfx_load("/rd/blaster.wav");
  sample2 = snd_sfx_load("/rd/pusher.wav");
  sample3 = snd_sfx_load("/rd/explode1.wav");

    mp3_start("/rd/bodenstaendig.mp3", 0);

  	while (1)
  	{
  		DeltaTime = (SDL_GetTicks()/1000) - lastTick;

  		//Handle events on queue
  		while (SDL_PollEvent(&e) != 0)
  		{
  			switch (e.type)
  			{
  			case SDL_KEYDOWN:
  				if (e.key.keysym.sym == SDLK_SPACE && player.alive == 1)
  				{
  					addBullet(player.pos.x + player.tex->w / 2 - 3, player.pos.y);
  				}
  				break;
  			}
  		}


  		updateExplo();
  		updateBullet();
  		updateEnemyBullet();
  		updateEnemies();

  		if(player.alive == 1)
  		updatePlayer(); //player

  		updateLogic();

  		int e;
  		for (e = 0; e < MAX_ENEMIES; e++) if (enemy[e]) {
  			enemy[e]->rect.x = currentFrame * 32;
  		}

  		SDL_BlitSurface(space, NULL, screen, NULL);
  		drawExplo();
  		drawEnemies();
  		drawBullet();
  		drawEnemyBullet();


  		if (player.alive == 1) {
  			//SDL_FillRect(screen, &player.hitbox, SDL_MapRGB(screen->format, 255, 0, 0));
  			SDL_BlitSurface(player.tex, NULL, screen, &player.pos);
  		}

  		//Update Screen
  		SDL_Flip(screen);

  		lastTick = (SDL_GetTicks()/1000);
  		SDL_Delay(30);
  	}

  	SDL_FreeSurface(player.tex);
  	SDL_FreeSurface(space);

  	//Pause
  	//SDL_Delay(2000);

  	//Free the loaded image
  	//SDL_FreeSurface( hello );

  	//Quit SDL
  	//SDL_Quit();

  	return 0;
  }