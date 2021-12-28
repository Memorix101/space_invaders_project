#include <acknex.h>
#include <default.c>

#define PRAGMA_PATH "rd";

PANEL* background1_pan = // display background
{
	bmap = "space3.bmp"; // this bitmap has 1024 x 768 pixels
	layer = 10;
	pos_x = 0;
	pos_y = 0;
	flags = SHOW;
}

PANEL* player_pan = // display player
{
	bmap = "player.bmp";
	layer = 30;
	pos_x = 0;
	pos_y = 0;
	flags = SHOW;	
}	

FONT* arial_font = "Arial#20b"; 

SOUND* blaster_wav = "blaster.wav";
PANEL* temp_bullet[10]; // temporary player bullet panel pointer

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

var quit = 0;
var spacekey_pressed = false;
var lasers = 0;

void player_update()
{
	if (key_space && spacekey_pressed == false) 
	{
		spacekey_pressed = true;
		
		if(lasers == 9)
		{
			lasers = 0;
		}
		
		str_cpy(temp_str,"lasers: ");
		str_cat_num(temp_str, "%.0f", lasers);
		
		if(temp_bullet[lasers] == NULL){
			temp_bullet[lasers] = pan_create("bmap = bullet.bmp;", 35);
			temp_bullet[lasers].pos_x = player_pan.pos_x + 22/2; // add half of player_pan's width
			temp_bullet[lasers].pos_y = player_pan.pos_y - 10;
			temp_bullet[lasers].flags |= SHOW;
			lasers++;
		}
		
		snd_play(blaster_wav, 100, 0);
	}
	else if (!key_space && spacekey_pressed == true) 
	{
		spacekey_pressed = false;
	}
	
	if(key_cul)
	{
		player_pan.pos_x -= 10;
	} 
	
	if(key_cur)
	{
		player_pan.pos_x += 10;
	}
	player_pan.pos_x = clamp(player_pan.pos_x,0,screen_size.x - 28); 
	
	var i;
	for(i = 0; i < 9; i++)
	{		
		if (temp_bullet[i] != NULL) // the bullet exists?
		{
			if (temp_bullet[i].pos_y > 0) // still flying upwards?
			{
				temp_bullet[i].pos_y -= 10; // then move it upwards by 10 pixels each frame
			}
			else // the bullet has reached the top of the screen?
			{
				reset(temp_bullet[i], SHOW); // then make it invisible
				temp_bullet[i] = NULL; // and free the panel pointer (prepare for a new bullet)
			}
		}
	}	
}

action rotate_plane()
{
	while (1)
	{
		my.pan = my.pan + 0.1;
		wait (1);
	}
}

void main()
{
	fps_max = 60; // limit the frame rate to 60 fps
	video_set(640, 480, 32, 2);
	
	player_pan.pos_x = 640/2 - 28/2;
	player_pan.pos_y = screen_size.y - 28;	

	while(quit != 1)
	{	
		
		player_update();
			
		wait (1);
	}
}