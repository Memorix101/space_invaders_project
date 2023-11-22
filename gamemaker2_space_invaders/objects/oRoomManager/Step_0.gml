/// @description Insert description here
// You can write your code in this editor


//show_debug_message("enemies_alive " + string(instance_number(oEnemy)));

if(instance_number(oEnemy) == 0){
	instance_activate_object(oWinUI);
	global.game_over = true;
}


if(global.game_over == true && keyboard_check_pressed(vk_enter)){
	room_restart();
	score = 0;
}