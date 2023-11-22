
if keyboard_check(vk_left)
{
    x = x - move_speed;
}
else if keyboard_check(vk_right)
{
    x = x + move_speed;
}

if keyboard_check_pressed(vk_space)
{
    with (instance_create_layer(x + 10, y - 15, "bullets", oPlayerBullet)){
		//other.bullet_number = i;
	}
}

x =	clamp(x, 0, 640 - self.sprite_width);