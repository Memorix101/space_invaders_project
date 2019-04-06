module Game

open Microsoft.Xna.Framework
open Microsoft.Xna.Framework.Graphics
open Microsoft.Xna.Framework.Input
open System.Collections.Generic

open Player
open Bullet
open Enemy
open Explosion
open System
open Microsoft.Xna.Framework.Media
open Microsoft.Xna.Framework.Audio

type Game1 () as this =
    inherit Game()
    do this.Window.Title <- "Space Invaders F# MonoGame" //Know DesktopGL issue :(
    do this.Content.RootDirectory <- "Content"
    let graphics = new GraphicsDeviceManager(this)
    do graphics.PreferredBackBufferWidth <- 640
    do graphics.PreferredBackBufferHeight <- 480

    let mutable spriteBatch = null
    let mutable background_tex2d: Texture2D = null
    let mutable gameover_ui: Texture2D = null
    let mutable win_ui: Texture2D = null
    let mutable scoreFont: SpriteFont = null
    let mutable score = 0
    let mutable rowCount = 0
    let mutable itemCount = 0
    let mutable stageClear = false
    let mutable player = Player(Vector2(300.0f, 450.0f))
    let lasers = List<Bullet>()
    let enemies = List<Enemy>()
    let explosions = List<Explosion>()
    let enemyLasers = List<Bullet>()

    let mutable music: Song = null
    let mutable snd_blaster: SoundEffect = null
    let mutable snd_blasterInstance: SoundEffectInstance = null
    let mutable snd_blasterEnemy: SoundEffect = null
    let mutable snd_blasterEnemyInstance: SoundEffectInstance = null
    let mutable snd_explo: SoundEffect = null
    let mutable snd_exploInstance: SoundEffectInstance = null

    let r = new Random();

    let spawnEnemies () =
        // create enemies
        for i in 0 .. 39 do 
            if i % 10 = 0
                then
                    itemCount <- 0
                    rowCount <- rowCount + 1

            itemCount <- itemCount + 1

            let mutable rn = r.Next(3, 20)
            //printf "rn:%d\n" rn
            enemies.Add(Enemy(Vector2((float32) itemCount * 40.0f, 40.0f * (float32) rowCount), itemCount, (float) rn, this.Content))

    let restart () =
        player.dead <- false
        player.position <- Vector2(640.0f / 2.0f - (float32) player.TileBoundingBox.Width / 2.0f, player.position.Y)
        lasers.Clear()
        enemies.Clear()
        enemyLasers.Clear()
        explosions.Clear()
        itemCount <- 0
        rowCount <- 0
        score <- 0
        stageClear <- false
        spawnEnemies()

    override this.Initialize() =
        printf "Space Invaders F# MonoGame\n"
        // create enemies
        spawnEnemies()

        base.Initialize()
    
    override this.LoadContent() =
        spriteBatch <- new SpriteBatch(this.GraphicsDevice)

        background_tex2d <- this.Content.Load<Texture2D>("space3")
        gameover_ui <- this.Content.Load<Texture2D>("gameover_ui")
        win_ui <- this.Content.Load<Texture2D>("win_ui")
        scoreFont <- this.Content.Load<SpriteFont>("font");
        player.loadResources(this.Content)
       
        music <- this.Content.Load<Song>("bodenstaendig")
        MediaPlayer.Play(music);
        MediaPlayer.IsRepeating <- true;

        snd_blaster <- this.Content.Load<SoundEffect>("blaster")
        snd_blasterEnemy <- this.Content.Load<SoundEffect>("pusher")
        snd_explo <- this.Content.Load<SoundEffect>("explode1")
 
    override this.Update(gameTime) =
        if GamePad.GetState(PlayerIndex.One).Buttons.Back = ButtonState.Pressed || Keyboard.GetState().IsKeyDown(Keys.Escape)
            then this.Exit()

        player.update(gameTime)

        if player.shoot
           then
           lasers.Add(Bullet(Vector2(player.position.X + (float32) player.TileBoundingBox.Width / 2.0f - 1.5f, player.position.Y - 32.0f), false, this.Content)); //1.5f is half of laser width ;)
           snd_blasterInstance <- snd_blaster.CreateInstance()
           snd_blasterInstance.Play() |> ignore

        if Keyboard.GetState().IsKeyDown(Keys.Enter) && player.dead = true || Keyboard.GetState().IsKeyDown(Keys.Enter) && stageClear = true
            then 
                restart()

        for i in enemies.Count-1 .. -1 .. 0 do
            enemies.Item(i).update(gameTime)     
            if enemies.Item(i).shoot = true 
                then
                    //printf "pew pew!\n"
                    enemyLasers.Add(Bullet(Vector2(enemies.Item(i).position.X + 32.0f / 2.0f - 4.5f, enemies.Item(i).position.Y + 9.0f), true, this.Content));
                    snd_blasterEnemyInstance <- snd_blasterEnemy.CreateInstance()
                    snd_blasterEnemyInstance.Play() |> ignore
            for l in lasers.Count-1 .. -1 .. 0 do 
                if lasers.Item(l).TileBoundingBox.Intersects(enemies.Item(i).TileBoundingBox) && enemies.Item(i).dead = false
                    then
                        //printf "BOOM!\n"
                        score <- score + 100
                        let spriteWidth = enemies.Item(i).TileBoundingBox.Width
                        explosions.Add(Explosion(Vector2(lasers.Item(l).position.X - (128.0f / 2.0f) + (float32) spriteWidth * 0.2f / 2.0f, lasers.Item(l).position.Y - 128.0f / 2.0f), this.Content))
                        lasers.RemoveAt(l)
                        enemies.Item(i).dead <- true //enemies.RemoveAt(i) //don't know how to fix this. could not find something like break or workaround
                        snd_exploInstance <- snd_explo.CreateInstance()
                        snd_exploInstance.Play() |> ignore

            if enemies.Item(i).dead
                then enemies.RemoveAt(i)

        for i in lasers.Count-1 .. -1 .. 0 do 
            lasers.Item(i).update(gameTime)

            if lasers.Item(i).position.Y <= 0.0f
                then lasers.RemoveAt(i);        

        for i in enemyLasers.Count-1 .. -1 .. 0 do 
            enemyLasers.Item(i).update(gameTime)

            if enemyLasers.Item(i).TileBoundingBox.Intersects(player.TileBoundingBox) && player.dead = false
                then
                    enemyLasers.RemoveAt(i);
                    explosions.Add(Explosion(Vector2(player.position.X - 128.0f / 2.0f, player.position.Y - 128.0f / 2.0f), this.Content))
                    player.dead <- true;
                    snd_exploInstance <- snd_explo.CreateInstance()
                    snd_exploInstance.Play() |> ignore

            if enemyLasers.Item(i).position.Y >= 480.0f
                then enemyLasers.RemoveAt(i);

        for i in explosions.Count-1 .. -1 .. 0 do 
            explosions.Item(i).update(gameTime)

        base.Update(gameTime)
 
    override this.Draw(gameTime) =
        this.GraphicsDevice.Clear Color.CornflowerBlue
        spriteBatch.Begin(SpriteSortMode.Deferred, BlendState.AlphaBlend)
        spriteBatch.Draw(background_tex2d, Vector2.Zero, Color.White)      
        player.draw(spriteBatch)
        for i in lasers do i.draw(spriteBatch)
        for i in enemyLasers do i.draw(spriteBatch)
        for i in enemies do i.draw(spriteBatch)
        for i in explosions do i.draw(spriteBatch)
        spriteBatch.DrawString(scoreFont, "Score: " + score.ToString("0000"), Vector2(640.0f - scoreFont.MeasureString("Score: 0000").X - 16.0f, 16.0f), Color.White);
        
        if enemies.Count = 0
            then 
                spriteBatch.Draw(win_ui, Vector2.Zero, Color.White)
                stageClear <- true
        elif player.dead = true
            then spriteBatch.Draw(gameover_ui, Vector2.Zero, Color.White)  

        spriteBatch.End()

        base.Draw(gameTime)