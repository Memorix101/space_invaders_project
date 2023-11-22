/// @description Insert description here
// You can write your code in this editor

if(move_left == false){
	x = x + move_speed;	
	if(x >= start_posx + 228){
		move_left = true;
	}	
} else {
	x = x - move_speed;
	if(x <= start_posx - 28){
		move_left = false;
	}	
}
