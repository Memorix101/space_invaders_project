module Player

open Microsoft.Xna.Framework
open Microsoft.Xna.Framework.Graphics
open Microsoft.Xna.Framework.Input
open Microsoft.Xna.Framework.Content

type Player (pos: Vector2) = class
    
    //private variables
    let mutable _TileBoundingBox: Rectangle = Rectangle(0, 0, 0, 0)
    let mutable _sprite: Texture2D = null
    let mutable keyState = KeyboardState()

    let mutable cbuttonState = GamePadState() 
    let mutable lastcbuttonState = GamePadState()
    let mutable cpadState = GamePadState()
    
    let move_speed: float32 = 350.0f

    //public variables
    member val dead = false with get, set
    member val shoot = false with get, set
    member val position = pos with get, set
    member this.TileBoundingBox = _TileBoundingBox

    //functions    
    member this.loadResources(con: ContentManager) =
        _sprite <- con.Load<Texture2D>("player");
        this.position <- Vector2(640.0f / 2.0f - (float32) _sprite.Bounds.Width / 2.0f, this.position.Y)
        _TileBoundingBox <- Rectangle((int)pos.X, (int) pos.Y, _sprite.Width, _sprite.Height)

    member this.input(gameTime: GameTime) =

        if this.dead = false 
            then

                cpadState <- GamePad.GetState(PlayerIndex.One)         
                lastcbuttonState <-  cbuttonState

                if Keyboard.GetState().IsKeyDown(Keys.Space) && keyState.IsKeyDown(Keys.Space) = false 
                    || GamePad.GetState(PlayerIndex.One).IsButtonDown(Buttons.A) && lastcbuttonState.IsButtonUp(Buttons.A)
                    then
                        this.shoot <- true
                    else
                        this.shoot <- false

                cbuttonState <- cpadState

                if Keyboard.GetState().IsKeyDown(Keys.A) || Keyboard.GetState().IsKeyDown(Keys.Left) || cpadState.ThumbSticks.Left.X <= -0.5f
                    then this.position <- Vector2(this.position.X - move_speed * (float32) gameTime.ElapsedGameTime.TotalSeconds, this.position.Y)

                if Keyboard.GetState().IsKeyDown(Keys.D) || Keyboard.GetState().IsKeyDown(Keys.Right) || cpadState.ThumbSticks.Left.X >= 0.5f
                    then this.position <- Vector2(this.position.X + move_speed * (float32) gameTime.ElapsedGameTime.TotalSeconds, this.position.Y)

                keyState <- Keyboard.GetState()

    member this.update(gameTime: GameTime) =
        this.input(gameTime)

        if this.dead = false 
            then           
            this.position <- Vector2(MathHelper.Clamp(this.position.X, 0.0f, (float32) 640 - (float32) _sprite.Width), 
                                    MathHelper.Clamp(this.position.Y, 0.0f, (float32) 480 - (float32) _sprite.Height));

        _TileBoundingBox <- Rectangle((int)this.position.X, (int) this.position.Y, _sprite.Width, _sprite.Height)

    member this.draw(batch: Microsoft.Xna.Framework.Graphics.SpriteBatch) =
        if this.dead = false
            then batch.Draw(_sprite, this.position, Color.White);
end