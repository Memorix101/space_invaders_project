use std::time::{Duration, Instant};

use crate::laser::{self, *};
use rand::Rng; // 0.8.0
use sdl2::{
    keyboard::Scancode,
    rect::Rect,
    render::{Texture, WindowCanvas},
    EventPump, mixer::Chunk,
};

#[derive(Debug)]
pub struct Enemy {
    pub position: Rect,
    pub rect: Rect,
    pub speed: i32,
    //texture: Texture,
    pub laserList: Vec<Laser>,
    curFrame: f32,
    shootTimer: i32,
    timer: f32,
    animationSpeed: f32,
    pub alive: bool,
    pub startPos: i32,
    goLeft: bool,
    pub rowPosID: i32,
    pos_x: f32
}

impl Enemy {
    pub fn new(x: i32, y: i32) -> Self {
        Enemy {
            position: Rect::new(x, y, 32, 32),
            rect: Rect::new(0, 0, 32, 32),
            speed: 2,
            laserList: Vec::new(),
            curFrame: 0.0,
            shootTimer: rand::thread_rng().gen_range(3..20), //2
            timer: 0.0,
            animationSpeed: 25.0,
            alive: true,
            startPos: 0,
            goLeft: false,
            rowPosID: 0,
            pos_x: 0.0
        }
    }

    pub fn update(&mut self, deltaTime: &f32, pusher_snd: &Chunk) {
        self.timer += deltaTime;
        //println!("timer: {}", self.timer);
        //println!("deltaTime: {}", deltaTime);

        self.curFrame += 25.0 * deltaTime;
        self.rect.x = self.curFrame as i32 * 32;

        if self.curFrame > 3.0 {
            self.curFrame = 0.0;
        }

        if self.timer >= self.shootTimer as f32 {
            self.laserList.push(laser::Laser::new(Rect::new(
                self.position.x + 12,
                self.position.y + 18,
                9,
                9,
            )));
            self.timer = 0.0;
            sdl2::mixer::Channel::all().play(&pusher_snd, 0).unwrap();
        }

        if self.goLeft == false {
            self.pos_x += self.speed as f32 * deltaTime;
            self.position.x += self.pos_x as i32;
        }

        if self.goLeft == true {
            self.pos_x -= self.speed as f32 * deltaTime;
            self.position.x -= self.pos_x as i32;
        }

        if self.position.x >= 640 - (self.position.width() as i32 + self.rowPosID) && !self.goLeft {
            self.goLeft = true;
        }

        if self.position.x <= self.startPos + self.position.width() as i32 && self.goLeft {
            self.goLeft = false;
        }
    }
}
