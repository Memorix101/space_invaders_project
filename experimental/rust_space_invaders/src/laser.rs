use sdl2::{
    keyboard::Scancode,
    rect::Rect,
    render::{Texture, WindowCanvas},
    EventPump,
};

#[derive(Debug)]
pub struct Laser {
    pub position: Rect,
    pub speed: i32,
    //texture: Texture,
    pub alive: bool,
}

impl Laser {
    pub fn new(pos: Rect) -> Self {
        Laser {
            position: Rect::new(pos.x, pos.y, pos.width(), pos.height()),
            speed: 5,
            alive: true,
        }
    }

    pub fn setPos(&mut self, dir: bool) {
      if dir == false
      {
        self.position.y -= self.speed;
      }
      else
      {
        self.position.y += self.speed;
      }        
    }
}
