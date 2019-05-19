#include <jo/jo.h>

static int space_tex = NULL;
static int player_tex = NULL;


void preload_assets(void)
{
	space_tex = jo_sprite_add_tga(JO_ROOT_DIR, "SPACE3.TGA", NULL);
	player_tex = jo_sprite_add_tga(JO_ROOT_DIR, "PLAYER.TGA", JO_COLOR_Transparent);
}

void draw(void)
{
	jo_printf(0, 0, jo_get_last_error());
	jo_printf(0, 0, "Use keyboard or gamepad to move the ship");
	jo_sprite_draw3D(space_tex, 0, 0, 500);
	jo_sprite_draw3D(player_tex, 0, 0, 500);
}


void jo_main(void)
{
	jo_core_init(JO_COLOR_Black);
	preload_assets();
	jo_core_add_callback(draw);
	jo_core_run();
}
