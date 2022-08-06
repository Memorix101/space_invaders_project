use sdl2::{
    keyboard::Scancode,
    rect::Rect,
    render::{Texture, WindowCanvas},
    EventPump, mixer::Chunk,
};

#[derive(Debug)]
pub struct Explo {
    pub position: Rect,
    pub rect: Rect,
    pub speed: i32,
    //texture: Texture,
    curFrame: f32,
    timer: f32,
    animationSpeed: f32,
    pub alive: bool,
    pos_x: f32
}

impl Explo {
    pub fn new(x: i32, y: i32) -> Self {
        Explo {
            position: Rect::new(x - 64, y - 64, 128, 128),
            rect: Rect::new(0, 0, 128, 128),
            speed: 2,
            curFrame: 0.0,
            timer: 0.0,
            animationSpeed: 25.0,
            alive: true,
            pos_x: 0.0,
        }
    }

    pub fn update(&mut self, deltaTime: &f32) {
        self.timer += deltaTime;
        //println!("timer: {}", self.timer);

        self.curFrame += 32.0 * deltaTime;
        self.rect.x = self.curFrame as i32 * 128;

        if self.curFrame > 16.0 {
            self.alive = false;
        }
    }
}