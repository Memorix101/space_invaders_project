module Explosion

open Microsoft.Xna.Framework
open Microsoft.Xna.Framework.Graphics
open Microsoft.Xna.Framework.Content

type Explosion (pos: Vector2, con: ContentManager) = class
    
    //private variables
    let mutable spriteRect: Rectangle = Rectangle(0, 0, 0, 0)
    let mutable _TileBoundingBox: Rectangle = Rectangle(0, 0, 0, 0)
    let mutable _sprite: Texture2D = null
    let animationSpeed = 25
    let mutable currentFrame: float = 0.0
    let mutable animationCompleted = false
    let mutable _pos = pos
    
    //constructor
    do
       _pos <- pos
       _sprite <- con.Load<Texture2D>("explode");
       _TileBoundingBox <- Rectangle((int)pos.X, (int) pos.Y, _sprite.Width, _sprite.Height)

    //public variables
    member val position = pos with get, set
    member this.TileBoundingBox = _TileBoundingBox

    //functions    
    member this.animator(gameTime: GameTime) =
        currentFrame <- currentFrame + (float) animationSpeed * (float) gameTime.ElapsedGameTime.TotalSeconds;
        let mutable clamp:float32 = MathHelper.Clamp((float32) currentFrame, 0.0f, 16.0f)
        currentFrame <- (float) clamp
        
        if currentFrame >= 16.0
           then animationCompleted <- true;

        spriteRect <- Rectangle((int)currentFrame * 128, 0, 128, 128);

    member this.update(gameTime: GameTime) =        
        this.animator(gameTime)     
        _TileBoundingBox <- Rectangle((int)this.position.X, (int) this.position.Y, _sprite.Width, _sprite.Height)

    member this.draw(batch: Microsoft.Xna.Framework.Graphics.SpriteBatch) =
        batch.Draw(_sprite, this.position, (System.Nullable<Rectangle>) spriteRect, Color.White);
end