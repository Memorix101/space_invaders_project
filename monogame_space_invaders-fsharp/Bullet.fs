module Bullet

open Microsoft.Xna.Framework
open Microsoft.Xna.Framework.Graphics
open Microsoft.Xna.Framework.Content

[<AllowNullLiteral>]
type Bullet (pos: Vector2, isEnemy: bool, con: ContentManager) = class
    
    //private variables
    let mutable _TileBoundingBox: Rectangle = Rectangle((int) pos.X, (int) pos.Y, 0, 0)
    let mutable _sprite: Texture2D = con.Load<Texture2D>("bullet")
    let mutable _enemyBullet = isEnemy
    let speed = 250; 

        //constructor
    do
        if _enemyBullet = false 
            then _sprite <- con.Load<Texture2D>("bullet");
            else _sprite <- con.Load<Texture2D>("enemy-bullet")
        
        _TileBoundingBox <- Rectangle((int)pos.X, (int) pos.Y, _sprite.Width, _sprite.Height)

    //public variables
    member val position = pos with get, set
    member val enemyBullet = _enemyBullet with get, set
    member this.TileBoundingBox = _TileBoundingBox
    member val dispose = false with get, set

    //functions
    member this.update(gameTime: GameTime) =
        if this.enemyBullet = false 
            then 
                this.position <- Vector2(this.position.X, this.position.Y - (float32) speed * (float32) gameTime.ElapsedGameTime.TotalSeconds)
            else
                this.position <- Vector2(this.position.X, this.position.Y + (float32) speed * (float32) gameTime.ElapsedGameTime.TotalSeconds)
        
        _TileBoundingBox <- Rectangle((int)this.position.X, (int) this.position.Y, _sprite.Width, _sprite.Height)

    member this.draw(batch: Microsoft.Xna.Framework.Graphics.SpriteBatch) =
        batch.Draw(_sprite, this.position, Color.White);
end