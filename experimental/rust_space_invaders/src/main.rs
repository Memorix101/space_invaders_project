use sdl2::event::Event;
use sdl2::keyboard::Keycode;
use sdl2::keyboard::Scancode;
use sdl2::pixels::Color;
use sdl2::rect::Rect;
use sdl2::render::{Texture, WindowCanvas};
// "self" imports the "image" module itself as well as everything else we listed
use sdl2::image::{self, InitFlag, LoadTexture};
use std::time::Duration;
use std::time::Instant;

mod enemy;
mod laser;
mod player;

fn main() -> Result<(), String> {
    let sdl_context = sdl2::init()?;
    let video_subsystem = sdl_context.video()?;

    let _image_context = image::init(InitFlag::PNG | InitFlag::JPG)?;

    let window = video_subsystem
        .window("rust-sdl2 demo", 640, 480)
        .position_centered()
        .build()
        .expect("could not initialize video subsystem");

    let mut canvas = window
        .into_canvas()
        .build()
        .expect("could not make a canvas");

    let timer = sdl_context.timer()?;

    let texture_creator = canvas.texture_creator();
    let player_sprite = texture_creator.load_texture("rd/player.png")?;
    let space_sprite = texture_creator.load_texture("rd/space3.png")?;
    let bullet_sprite = texture_creator.load_texture("rd/bullet.png")?;
    let ebullet_sprite = texture_creator.load_texture("rd/enemy-bullet.png")?;
    let enemy_sprite = texture_creator.load_texture("rd/invader32x32x4.png")?;

    let mut player = player::Player::new();

    let mut enemyVec: Vec<enemy::Enemy> = Vec::new();
    let mut itemCount = 0;
    let mut rowCount = 0;
    for e in 0..40 {
        if e % 10 == 0
		{
			itemCount = 0;
			rowCount += 1;
		}
        itemCount += 1;
        enemyVec.push(enemy::Enemy::new(itemCount * 40, 40 * rowCount ));
        enemyVec[e].startPos = itemCount * 40;
        enemyVec[e].rowPosID = 40 * (11 - itemCount);
    }

    let mut event_pump = sdl_context.event_pump()?;
    let mut lastTick: f32 = 0.0;
    let start = Instant::now();

    let mut i = 0;
    'running: loop {
        i = (i + 1) % 255;
        canvas.set_draw_color(Color::RGB(i, 64, 255 - i));
        canvas.clear();
        for event in event_pump.poll_iter() {
            match event {
                Event::Quit { .. }
                | Event::KeyDown {
                    keycode: Some(Keycode::Escape),
                    ..
                } => {
                    break 'running;
                }
                /*Event::KeyDown { keycode: Some(Keycode::Left), .. } => {
                    player.position.x -= player.speed;
                },*/
                /*Event::KeyDown { keycode: Some(Keycode::Right), .. } => {
                    player.position.x += player.speed;
                },*/
                _ => {}
            }
        }

        let ticks = timer.ticks() as i32;
        let deltaTime = ticks as f32 / 1000.0 - lastTick;

        // The rest of the game loop goes here...
        if player.alive == true
        {
            player.update(&event_pump);
        }   

        for e in (0..enemyVec.len()).rev() {
            //let dt = deltaTime as i32;
            if enemyVec[e].alive == true {
                enemyVec[e].update(&deltaTime);

                for b in (0..player.laserList.len()).rev() {
                    if e != enemyVec.len() {
                        if enemyVec[e]
                            .position
                            .has_intersection(player.laserList[b].position)
                            && player.laserList[b].alive == true
                        {
                            player.laserList[b].alive = false;
                            //player.laserList.remove(b);
                            //enemyVec.remove(e);
                            enemyVec[e].alive = false;
                        }
                    }
                }
            }
        }

        // Rendering routine
        canvas.set_draw_color(Color::RGB(0, 0, 0));
        canvas.clear();

        canvas.copy(&space_sprite, None, None)?;

        if player.alive == true
        {
            canvas.copy(&player_sprite, None, player.position)?;
        }       

        for e in (0..enemyVec.len()).rev() {
            if enemyVec[e].alive == true 
            {
                canvas.copy(&enemy_sprite, enemyVec[e].rect, enemyVec[e].position)?;
            }

            for l in (0..enemyVec[e].laserList.len()).rev() {
                enemyVec[e].laserList[l].setPos(true);
                canvas.copy(&ebullet_sprite, None, enemyVec[e].laserList[l].position)?;
                if enemyVec[e].laserList[l].position.has_intersection(player.position) && player.alive == true {
                    player.alive = false;
                    enemyVec[e].laserList[l].alive = false;
                }
            }
        }

        for l in (0..player.laserList.len()).rev() {
            player.laserList[l].setPos(false);
            canvas.copy(&bullet_sprite, None, player.laserList[l].position)?;
            if player.laserList[l].position.y <= 10 && player.laserList[l].alive == true {
                player.laserList[l].alive = false;
                //player.laserList.remove(l);
            }
        }

        canvas.present();

        lastTick = ticks as f32 / 1000.0;
        //std::thread::sleep(Duration::from_millis(100));
        std::thread::sleep(Duration::new(0, 1_000_000_000u32 / 60));
    }

    Ok(())
}
