module Enemy

open Microsoft.Xna.Framework
open Microsoft.Xna.Framework.Graphics
open Microsoft.Xna.Framework.Content

[<AllowNullLiteral>]
type Enemy (pos: Vector2, rowid: int, maxShootTime: float, con: ContentManager) = class
    
    //private variables
    let mutable spriteRect: Rectangle = Rectangle(0, 0, 0, 0)
    let mutable _TileBoundingBox: Rectangle = Rectangle(0, 0, 0, 0)
    let mutable _sprite: Texture2D = null
    let moveSpeed = 50
    let animationSpeed = 25
    let mutable currentFrame: float = 0.0
    let mutable startPos: float = 0.0
    let mutable goLeft = false
    let mutable shootTimer: float = 0.0
    let mutable _shootTimeLimit = maxShootTime
    let mutable _pos = pos
    let mutable _rowid = 0
    
    //constructor
    do
       startPos <- (float) _pos.X
       _rowid <- 40 * (11 - rowid);
       shootTimer <- 0.0;
       _shootTimeLimit <- maxShootTime;
       _sprite <- con.Load<Texture2D>("invader32x32x4");
       _TileBoundingBox <- Rectangle((int)pos.X, (int) pos.Y, _sprite.Width, _sprite.Height)

    //public variables
    member val dead = false with get, set
    member val shoot = false with get, set
    member val position = pos with get, set
    member this.TileBoundingBox = _TileBoundingBox

    //functions        
    member this.animator(gameTime: GameTime) =
        currentFrame <- currentFrame + (float) animationSpeed * (float) gameTime.ElapsedGameTime.TotalSeconds;
        let mutable clamp:float32 = MathHelper.Clamp((float32) currentFrame, 0.0f, 4.0f)
        currentFrame <- (float) clamp
        
        if currentFrame >= 4.0
           then currentFrame <- 0.0;

        spriteRect <- Rectangle((int)currentFrame * 32, 0, 32, 32);

    member this.update(gameTime: GameTime) =
        if this.dead = false then
            this.animator(gameTime)
        
            if goLeft = false
            then
                 this.position <- Vector2((float32) this.position.X + (float32) moveSpeed * (float32) gameTime.ElapsedGameTime.TotalSeconds, this.position.Y)

            if goLeft
                then
                this.position <- Vector2((float32) this.position.X - (float32) moveSpeed * (float32) gameTime.ElapsedGameTime.TotalSeconds, this.position.Y)

            if this.position.X >= 640.0f - ((float32) spriteRect.Width + (float32) _rowid) && goLeft = false
                then
                goLeft <- true;

            if this.position.X <= (float32) startPos + (float32) spriteRect.Width && goLeft
                then
                goLeft <- false;

            shootTimer <- shootTimer + 1.0 * (float) gameTime.ElapsedGameTime.TotalSeconds;

            if shootTimer >= _shootTimeLimit 
                then
                    shootTimer <- 0.0;
                    this.shoot <- true
                else    
                    this.shoot <- false

            _TileBoundingBox <- Rectangle((int)this.position.X, (int) this.position.Y, _sprite.Width, _sprite.Height)

    member this.draw(batch: Microsoft.Xna.Framework.Graphics.SpriteBatch) =
        if this.dead = false
            then
                batch.Draw(_sprite, this.position, (System.Nullable<Rectangle>) spriteRect, Color.White);
end