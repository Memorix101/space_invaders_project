#include <acknex.h>
#include <default.c>
#include <windows.h>

#define PRAGMA_PATH "rd";

ENTITY* player_ent;
ENTITY* bullets_ent;
ENTITY* enemy_bullets_ent;
ENTITY* enemies_ent;
ENTITY* explode1_ent;

SOUND* blaster_wav = "blaster.wav";
SOUND* pusher_wav = "pusher.wav";
SOUND* explode1_wav = "explode1.wav";

STRING* score_str = "";

var score = 0;
var spacekey_pressed = false;
var xbox_a_pressed = false;
var go_left = false;
var shoot_counter = 0;
var gameover = false;
var rnd_shoot = 0;

PANEL* fmg_pan =
{
  layer = 9;
  pos_x = 0; 
  pos_y = 0;
  bmap = "fmg_splash.tga";
  flags = SHOW;
}

PANEL* you_win_pan =
{
	layer = 9;
  pos_x = 0; 
  pos_y = 0;
  bmap = "win_ui.tga";
  //flags = SHOW;
}

PANEL* gameover_pan =
{
	layer = 9;
  pos_x = 0; 
  pos_y = 0;
  bmap = "gameover_ui.tga";
  //flags = SHOW;
}

FONT* vermin_font = "Vermin Vibes 1989#24";

TEXT* score_txt =	
{
  layer = 10;
  pos_x = 450;
  pos_y = 20;
  font = vermin_font;
  string = score_str;
  flags = SHOW;
} 

// debugging stuff
STRING* temp_str = "";
TEXT* string_txt =
{
       pos_x = 20;
       pos_y = 20;
       string(temp_str);
       layer = 11;
       flags = SHOW;
}

action explode1_action()
{
	my.ambient = 100;
	my.group = 2; // ignores collison with all entities with the same group value
	my.roll = 180;
	my.pan = 90;
	 
	while (my != NULL) { 
	
		my.frame += 0.8 * time_step;
		if (my.frame >= 16) ent_remove(my);
	
		wait (1);
  	}

} 

void enemy_bullet_action()
{
	my.ambient = 100;
	my.group = 2; // ignores collison with all entities with the same group value
	
	while (my != NULL) 
	{
		c_ignore(2,3); // 2 - laser, explosion, 3 - enemy
		//my.z += 15 * time_step; 
		c_move(me,vector(0,0, -40*time_step),NULL, IGNORE_PUSH);
		
		if (HIT_TARGET)  // collided? 
		{
			explode1_ent = ent_create ("explode+16.tga", vector(my.x, -64, my.z), explode1_action);
			snd_play(explode1_wav, 100, 0);
			ent_remove(my); // deletes player
			ent_remove(you); // deletes laser
			gameover = true;
			set(gameover_pan, SHOW);
			score += 100;
			break;
		}
		
		if(my.z > 270)
		{
			ent_remove(my);
		}
		
		wait (1);
	}
}

void bullet_action()
{
	my.ambient = 100;
	my.group = 2; // runs through all entities with the same group value
	
	while (my != NULL) 
	{
		//my.z += 15 * time_step; 
		c_move(me,vector(0,0, 40*time_step),NULL, IGNORE_PUSH);
		
		if (HIT_TARGET)  // collided? 
		{
			explode1_ent = ent_create ("explode+16.tga", vector(my.x, -64, my.z), explode1_action);
			snd_play(explode1_wav, 100, 0);
			ent_remove(my); // deletes enemy
			ent_remove(you); // deletes laser
			score += 100;
			break;
		}
		
		if(my.z > 270)
		{
			ent_remove(my);
		}
		
		wait (1);
	}
}

action player_action()
{	
	my.ambient = 100;
	
	while (my != NULL)
	{
		
		if(key_cul || joy_raw.x < -150 || joy_hat == 270) 
		{
			my.x -= 30 * time_step;
		} 		
		else if(key_cur || joy_raw.x > 150 || joy_hat == 90)
		{
			my.x += 30 * time_step;
		}
			
		my.x = clamp(my.x,-290,290); 
		
		if (key_space && spacekey_pressed == false) 
		{
			spacekey_pressed = true;
			bullets_ent	= ent_create ("bullet.bmp", vector(my.x, my.y, 20+my.z + 10), bullet_action); 
			snd_play(blaster_wav, 100, 0);
		}
		else if (!key_space && spacekey_pressed == true) 
		{
			spacekey_pressed = false;
		}
		
		if (joy_1 && xbox_a_pressed == false) 
		{
			xbox_a_pressed = true;
			bullets_ent	= ent_create ("bullet.bmp", vector(my.x, my.y, 20+my.z + 10), bullet_action); 
			snd_play(blaster_wav, 100, 0);
		}
		else if (!joy_1 && xbox_a_pressed == true) 
		{
			xbox_a_pressed = false;
		}		
		
		wait (1);
	}
}


action enemy_action()
{
	my.ambient = 100;
	my.roll = 180;
	my.pan = 90;
	my.group = 3;
	rnd_shoot = random(60)+60;	
		
	while (my != NULL) {
	
		my.frame += 0.8 * time_step;
		if (my.frame >= 4) my.frame -= 3;
		
		if(my.x >= 290)
		{
			go_left = true;
		}
		else if(my.x < -290)
		{
			go_left = false;
		}
		
		if(go_left)
		{
			my.x -= 5 * time_step;
		}
		else
		{
			my.x += 5 * time_step;
		}	
		
		if(shoot_counter >= rnd_shoot)
		{
			enemy_bullets_ent	= ent_create ("enemy-bullet.bmp", vector(my.x, my.y, my.z), enemy_bullet_action); 
			snd_play(pusher_wav, 100, 0);
			shoot_counter = 0;
		}
		
		shoot_counter += 0.1 * time_step;
		
		wait (1);
  	}

} 

function init_enemies()
{
	var enemy_row = 0;
	var enemy_count = 0;
	var i;
	for(i = 0; i < 40; i++)
	{
		if(enemy_count % 10 == 0)
		{
			enemy_row += 1;
			enemy_count = 0;
		}		
		enemies_ent	= ent_create ("invader32x32+4.tga", vector(-200+enemy_count*40, -32, 250 - enemy_row*40), enemy_action); 
		enemy_count += 1;
	}	
}

function main()
{
	fps_max = 60;	// limit the frame rate to 60 fps 
	video_set(640, 480, 32, 2);	
		
	AddFontResource("rd/vermin_vibes_1989.ttf"); // neat trick from AUM 114 how to use TTF without installing it
	wait(1);
	
	//set(score_txt, SHOW); // invisible
	
	// setup camera
	//camera.bg = pixel_for_vec(COLOR_BLACK,0,8888); // clear view to transparent black
	camera.arc = 50;	

	level_load ("game.wmb");
	
	media_loop("rd/bodenstaendig.ogg", NULL, 100);
	
	wait(100);
	reset(fmg_pan, SHOW); // invisible
	
	player_ent	= ent_create ("player.bmp", vector(0, -32, -150), player_action);
	init_enemies();
	
	while(1) // update
	{
		str_cpy(score_str,"Score: ");
		str_cat_num(score_str,"%04.0f", score);
		
		//debug stuff
		/*str_cpy(temp_str,"hat: ");
		str_cat_num(temp_str, "%.0f", joy_hat);*/
		
		if(score >= 4000)
		{
			gameover = true;
			set(you_win_pan, SHOW);
		}
		
		if(gameover == true && key_enter || gameover == true && joy_8)
		{
			//reset
			gameover = false;
			score = 0;
			level_load ("game.wmb");
			player_ent	= ent_create ("player.bmp", vector(0, -32, -150), player_action);
			init_enemies();
			reset(gameover_pan, SHOW); // invisible
			reset(you_win_pan, SHOW); // invisible
		}
		
		wait(1);
	}
	
}