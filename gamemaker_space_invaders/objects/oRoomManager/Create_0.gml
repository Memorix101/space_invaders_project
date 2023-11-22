/// @description Insert description here
// You can write your code in this editor


global.game_over = false;

// title screen
for (var a = 0; a < 4; a++) // rows
{		
		for (var b = 0; b < 10; b++) // enemies per line
		{
			instance_create_layer(b * 40 + 20, 40 * a + 64, "enemies", oEnemy);	
			show_debug_message("Enemy created " + string(b) + " " + string(a));
		}
}