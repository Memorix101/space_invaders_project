use sdl2::event::Event;
use sdl2::keyboard::Keycode;
use sdl2::keyboard::Scancode;
use sdl2::mixer::DEFAULT_FORMAT;
use sdl2::pixels::Color;
use sdl2::rect::Rect;
use sdl2::render::{Texture, WindowCanvas};
// "self" imports the "image" module itself as well as everything else we listed
use sdl2::audio::{AudioCallback, AudioSpecDesired};
use sdl2::image::{self, InitFlag as InitFlagImage, LoadTexture};
use sdl2::mixer::{InitFlag as InitFlagMixer, AUDIO_S16LSB, DEFAULT_CHANNELS};
use std::env;
use std::path::Path;
use std::time::Duration;
use std::time::Instant;

mod enemy;
mod explo;
mod laser;
mod player;

fn reset(exploVec: &mut Vec<explo::Explo>, player: &mut player::Player, enemyVec: &mut Vec<enemy::Enemy>) {
    player.laserList.clear();  
    player.score = 0;
    player.alive = true;
    enemyVec.clear();
    exploVec.clear();

    let mut itemCount = 0;
    let mut rowCount = 0;
    for e in 0..40 {
        if e % 10 == 0 {
            itemCount = 0;
            rowCount += 1;
        }
        itemCount += 1;
        enemyVec.push(enemy::Enemy::new(itemCount * 40, 40 * rowCount));
        enemyVec[e].startPos = itemCount * 40;
        enemyVec[e].rowPosID = 40 * (11 - itemCount);
    }
}

fn main() -> Result<(), String> {
    let sdl_context = sdl2::init()?;
    let video_subsystem = sdl_context.video()?;
    let image_context = image::init(InitFlagImage::PNG | InitFlagImage::JPG)?;
    let ttf_context = sdl2::ttf::init().map_err(|e| e.to_string())?;
    let audio_context = sdl_context.audio()?;
    let mixer_context = sdl2::mixer::init(
        InitFlagMixer::MP3 | InitFlagMixer::FLAC | InitFlagMixer::MOD | InitFlagMixer::OGG,
    )?;

    sdl2::mixer::open_audio(44100, DEFAULT_FORMAT, DEFAULT_CHANNELS, 1024)?;
    sdl2::mixer::allocate_channels(24);

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
    let explo_sprite = texture_creator.load_texture("rd/explode.png")?;

    let gameover_sprite = texture_creator.load_texture("rd/gameover_ui.png")?;
    let win_sprite = texture_creator.load_texture("rd/win_ui.png")?;

    let music = sdl2::mixer::Music::from_file("rd/bodenstaendig.ogg")?;
    let blaster_snd = sdl2::mixer::Chunk::from_file("rd/blaster.ogg")?;
    let pusher_snd = sdl2::mixer::Chunk::from_file("rd/pusher.ogg")?;
    let explo_snd = sdl2::mixer::Chunk::from_file("rd/explode1.ogg")?;

    let mut player = player::Player::new();

    let mut font = ttf_context.load_font("rd/vermin_vibes_1989.ttf", 24)?;

    let mut exploVec: Vec<explo::Explo> = Vec::new();

    let mut enemyVec: Vec<enemy::Enemy> = Vec::new();
    let mut itemCount = 0;
    let mut rowCount = 0;
    for e in 0..40 {
        if e % 10 == 0 {
            itemCount = 0;
            rowCount += 1;
        }
        itemCount += 1;
        enemyVec.push(enemy::Enemy::new(itemCount * 40, 40 * rowCount));
        enemyVec[e].startPos = itemCount * 40;
        enemyVec[e].rowPosID = 40 * (11 - itemCount);
    }

    let mut event_pump = sdl_context.event_pump()?;
    let mut lastTick: f32 = 0.0;
    let start = Instant::now();

    music.play(-1);
    sdl2::mixer::Music::set_volume(128);

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
        if player.alive == true {
            player.update(&event_pump, &blaster_snd);
        }

        for e in (0..enemyVec.len()).rev() {
            //let dt = deltaTime as i32;
            if enemyVec[e].alive == true {
                enemyVec[e].update(&deltaTime, &pusher_snd);

                for b in (0..player.laserList.len()).rev() {
                    if e != enemyVec.len() {
                        if enemyVec[e]
                            .position
                            .has_intersection(player.laserList[b].position)
                            && player.laserList[b].alive == true
                            && enemyVec[e].alive == true
                        {
                            player.laserList[b].alive = false;
                            //player.laserList.remove(b);
                            //enemyVec.remove(e);
                            enemyVec[e].alive = false;
                            player.score += 100;
                            exploVec.push(explo::Explo::new(
                                enemyVec[e].position.x,
                                enemyVec[e].position.y,
                            ));
                            sdl2::mixer::Channel::all().play(&explo_snd, 0);
                        }
                    }
                }
            }
        }

        // Rendering routine
        canvas.set_draw_color(Color::RGB(0, 0, 0));
        canvas.clear();

        canvas.copy(&space_sprite, None, None)?;

        if player.score >= 4000 {
            canvas.copy(&win_sprite, None, None)?;

            if event_pump
                .keyboard_state()
                .is_scancode_pressed(Scancode::Return)
            {
                reset(&mut exploVec, &mut player, &mut enemyVec);
            }
        }

        if player.alive == true {
            canvas.copy(&player_sprite, None, player.position)?;
        } else {
            canvas.copy(&gameover_sprite, None, None)?;

            if event_pump
                .keyboard_state()
                .is_scancode_pressed(Scancode::Return)
            {
                reset(&mut exploVec, &mut player, &mut enemyVec);
            }
        }

        for e in (0..enemyVec.len()).rev() {
            if enemyVec[e].alive == true {
                canvas.copy(&enemy_sprite, enemyVec[e].rect, enemyVec[e].position)?;
            }

            for l in (0..enemyVec[e].laserList.len()).rev() {
                enemyVec[e].laserList[l].setPos(true);
                canvas.copy(&ebullet_sprite, None, enemyVec[e].laserList[l].position)?;
                if enemyVec[e].laserList[l]
                    .position
                    .has_intersection(player.position)
                    && player.alive == true
                {
                    player.alive = false;
                    enemyVec[e].laserList[l].alive = false;
                    exploVec.push(explo::Explo::new(
                        enemyVec[e].position.x,
                        enemyVec[e].position.y,
                    ));
                    sdl2::mixer::Channel::all().play(&explo_snd, 0);
                }
            }
        }

        for e in (0..exploVec.len()).rev() {
            if exploVec[e].alive == false {
                exploVec.remove(e);
                //println!("exploVec: {}", exploVec.len());
            }
        }

        for l in (0..exploVec.len()).rev() {
            exploVec[l].update(&deltaTime);
            canvas.copy(&explo_sprite, exploVec[l].rect, exploVec[l].position)?;
        }

        for l in (0..player.laserList.len()).rev() {
            player.laserList[l].setPos(false);
            canvas.copy(&bullet_sprite, None, player.laserList[l].position)?;
            if player.laserList[l].position.y <= 10 && player.laserList[l].alive == true {
                player.laserList[l].alive = false;
                //player.laserList.remove(l);
            }
        }

        // maybe there is a better way ...
        let score_surface = font
            .render(format!("{}{:04}", "SCORE: ", player.score).as_str()) // https://stackoverflow.com/a/50458253/9296008
            .blended(Color::RGBA(255, 255, 255, 255))
            .map_err(|e| e.to_string())?;
        let score_texture = texture_creator
            .create_texture_from_surface(&score_surface)
            .map_err(|e| e.to_string())?;

        canvas.copy(
            &score_texture,
            None,
            Rect::new(
                640 - score_texture.query().width as i32 - 20,
                20,
                score_texture.query().width,
                score_texture.query().height,
            ),
        )?;

        canvas.present();

        lastTick = ticks as f32 / 1000.0;
        //std::thread::sleep(Duration::from_millis(100));
        std::thread::sleep(Duration::new(0, 1_000_000_000u32 / 60));
    }

    Ok(())
}
