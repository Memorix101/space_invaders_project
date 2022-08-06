use sdl2::{
    keyboard::Scancode,
    rect::Rect,
    render::{Texture, WindowCanvas},
    EventPump,
};

use crate::laser::{self, *};

#[derive(Debug)]
pub struct Player {
    pub position: Rect,
    pub speed: i32,
    //texture: Texture,
    pub laserList: Vec<Laser>,
    fire_pressed: bool,
    pub alive: bool
}

impl Player {
    pub fn new() -> Self {
        Player {
            position: Rect::new(640 / 2 - 28 / 2, 450, 28, 21),
            speed: 5,
            laserList: Vec::new(),
            fire_pressed: false,
            alive: true,
        }
    }

    pub fn update(&mut self, event_pump: &EventPump) {
        if event_pump
            .keyboard_state()
            .is_scancode_pressed(Scancode::Left)
        {
            self.position.x -= self.speed;
        } else if event_pump
            .keyboard_state()
            .is_scancode_pressed(Scancode::Right)
        {
            self.position.x += self.speed;
        }

        self.position.x = self.position.x.clamp(0, 640 - 28);

        if event_pump
            .keyboard_state()
            .is_scancode_pressed(Scancode::Space)
            && self.fire_pressed == false
        {
            self.fire_pressed = true;
            self.laserList.push(laser::Laser::new(Rect::new(
                self.position.x + 8,
                self.position.y - 30,
                6,
                36,
            )));
        } else if event_pump
            .keyboard_state()
            .is_scancode_pressed(Scancode::Space)
            == false
        {
            self.fire_pressed = false;
        }

        self.remove();
    }

    pub fn remove(&mut self) {
        for l in (0..self.laserList.len()).rev() {
            if self.laserList[l].alive == false {
                if l != self.laserList.len() {
                    self.laserList.remove(l);
                }
            }
        }
    }
}
