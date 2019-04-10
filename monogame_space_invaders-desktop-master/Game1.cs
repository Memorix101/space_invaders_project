using System;
using System.Collections.Generic;
using System.Diagnostics;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Audio;
using Microsoft.Xna.Framework.Graphics;
using Microsoft.Xna.Framework.Input;
using Microsoft.Xna.Framework.Media;

namespace SpaceInvaders_Desktop
{
    /// <summary>
    /// This is the main type for your game.
    /// </summary>
    public class Game1 : Game
    {
        GraphicsDeviceManager graphics;
        SpriteBatch spriteBatch;

        Texture2D fmg_logo;
        Texture2D background;
        Texture2D frameBackground;
        Texture2D arcade;
        Texture2D gameover_ui;
        Texture2D win_ui;

        Player player;
        List<Laser> laser;
        List<EnemyLaser> enemyLaser;
        List<Explosion> explosion;

        List<Enemy> enemy;
        int rowCount;
        int itemCount;

        bool gamestart = false;
        float timer = 0f;
        private bool gameover = false;

        Viewport backgroundViewport; //total screenszie
        Viewport gameViewport;

        SpriteFont Font1;

        string msg = "";
        bool enemyHit = false;
        int score;

        Song music;

        SoundEffect snd_blaster;
        SoundEffect snd_blasterEnemy;
        SoundEffect snd_explo;

        public Game1()
        {
            graphics = new GraphicsDeviceManager(this);
            Content.RootDirectory = "Content";
            graphics.PreferredBackBufferHeight = 1080;
            graphics.PreferredBackBufferWidth = 1920;
        }

        private void restart()
        {
            gamestart = false;
            enemy.Clear();
            player.Position = new Vector2(640.0f / 2.0f - player.TileBoundingBox.Width / 2.0f, player.Position.Y);
            laser.Clear();
            enemyLaser.Clear();
            explosion.Clear();
            itemCount = 0;
            rowCount = 0;
            score = 0;
            spawnEnemies();
            player.dead = false;
            gameover = false;
            gamestart = true;
        }

        private void spawnEnemies()
        {
            Random r = new Random();

            // create enemies
            for (int i = 0; i < 40; i++)
            {
                if (i % 10 == 0)
                {
                    itemCount = 0;
                    rowCount++;
                }

                itemCount++;
                enemy.Add(new Enemy(new Vector2(itemCount * 40, 40 * rowCount), itemCount, r.Next(3, 20)));

                // Debug.WriteLine("row:" + rowCount);
                // Debug.WriteLine("item:" + itemCount * 40);
            }

            foreach (Enemy e in enemy)
                e.LoadResources(Content);
        }

        protected override void Initialize()
        {
            player = new Player();

            enemy = new List<Enemy>();
            laser = new List<Laser>();
            explosion = new List<Explosion>();
            enemyLaser = new List<EnemyLaser>();
            
            // create enemies
            spawnEnemies();

            base.Initialize();
        }

        protected override void LoadContent()
        {
            // Create a new SpriteBatch, which can be used to draw textures.
            spriteBatch = new SpriteBatch(GraphicsDevice);

            backgroundViewport = GraphicsDevice.Viewport;
            gameViewport = backgroundViewport;
            gameViewport.Width = 640;
            gameViewport.Height = 480;

            player.LoadResources(Content);
            
            fmg_logo = Content.Load<Texture2D>("fmg_15b");
            background = Content.Load<Texture2D>("space3");
            frameBackground = Content.Load<Texture2D>("BGPattern_blue");
            arcade = Content.Load<Texture2D>("arcade");
            gameover_ui = Content.Load<Texture2D>("gameover_ui");
            win_ui = Content.Load<Texture2D>("win_ui");
            Font1 = Content.Load<SpriteFont>("font");

            music = Content.Load<Song>("bodenstaendig");
            MediaPlayer.Play(music);
            MediaPlayer.IsRepeating = true;

            snd_blaster = Content.Load<SoundEffect>("blaster");
            snd_blasterEnemy = Content.Load<SoundEffect>("pusher");
            snd_explo = Content.Load<SoundEffect>("explode1");
        }

        protected override void UnloadContent()
        {
            // TODO: Unload any non ContentManager content here
        }

        protected override void Update(GameTime gameTime)
        {

            timer += 1 * (float)gameTime.ElapsedGameTime.TotalSeconds;

            if (timer >= 3f)
                gamestart = true;

            if (gamestart)
            {
                //Update Windowsize
                gameViewport.X = (int)Window.ClientBounds.Width / 2 - gameViewport.Width / 2;
                gameViewport.Y = (int)Window.ClientBounds.Height / 2 - gameViewport.Height / 2;

                player.Update(gameTime);

                if (player.shoot)
                {
                    Debug.WriteLine("SHOOT");
                    laser.Add(new Laser(new Vector2(player.Position.X + player.SpriteWidth / 2 - 1.5f, player.Position.Y - 32), Content)); //1.5f is half of laser width ;)
                    snd_blaster.Play();
                }

                for (int i = laser.Count - 1; i >= 0; i--)
                {
                    laser[i].Update(gameTime);

                    if (laser[i].Position.Y <= 0)
                        laser.RemoveAt(i);
                }

                for (int e = enemy.Count - 1; e >= 0; e--)
                {
                    enemy[e].Update(gameTime);

                    if (enemy[e].shoot)
                    {
                        enemyLaser.Add(new EnemyLaser(new Vector2(enemy[e].Position.X + 32 / 2 - 4.5f, enemy[e].Position.Y + 9), Content));
                        snd_blasterEnemy.Play();
                    }

                    for (int l = laser.Count - 1; l >= 0; l--)
                    {
                        if (laser[l].TileBoundingBox.Intersects(enemy[e].TileBoundingBox))
                        {
                            // Debug.WriteLine("BOOM!");      
                            score += 100;
                            explosion.Add(new Explosion(new Vector2(laser[l].Position.X - 128 / 2, laser[l].Position.Y - 128 / 2), Content));
                            laser.RemoveAt(l);
                            enemy.RemoveAt(e);
                            snd_explo.Play();
                            break;
                        }
                    }
                }

                for (int i = enemyLaser.Count - 1; i >= 0; i--)
                {
                    enemyLaser[i].Update(gameTime);

                    if (enemyLaser[i].TileBoundingBox.Intersects(player.TileBoundingBox))
                    {
                        enemyLaser[i].dispose = true;
                        explosion.Add(new Explosion(new Vector2(player.Position.X - 128 / 2, player.Position.Y - 128 / 2), Content));
                        player.dead = true;
                        snd_explo.Play();
                        //GamePad.SetVibration(PlayerIndex.One, 1.0f, 1.0f);
                    }

                    if (enemyLaser[i].Position.Y >= 480)
                        enemyLaser[i].dispose = true;
                }

                for (int i = enemyLaser.Count - 1; i >= 0; i--)
                {
                    if (enemyLaser[i].dispose)
                        enemyLaser.RemoveAt(i);
                }

                for (int i = explosion.Count - 1; i >= 0; i--)
                {
                    explosion[i].Update(gameTime);

                    if (explosion[i].animationCompleted)
                        explosion.RemoveAt(i);
                }

                /*if (enemy.Count == 0)
                {
                    msg = "You Win!";
                }
                else if (player.dead)
                {
                    msg = "Game Over!";
                }*/

                if (GamePad.GetState(PlayerIndex.One).IsButtonDown(Buttons.Back) || Keyboard.GetState().IsKeyDown(Keys.Escape))
                    Exit();

               if (GamePad.GetState(PlayerIndex.One).IsButtonDown(Buttons.Start) && gameover)
                {
                    restart();
                }
            }

            base.Update(gameTime);
        }

        protected override void Draw(GameTime gameTime)
        {
            GraphicsDevice.Viewport = backgroundViewport;
            GraphicsDevice.Clear(Color.Black);

            spriteBatch.Begin(SpriteSortMode.Deferred, BlendState.AlphaBlend);

            if (gamestart)
            {
                spriteBatch.Draw(frameBackground, new Rectangle(0, 0, GraphicsDevice.Viewport.Width, GraphicsDevice.Viewport.Height), null, new Color(0.25f, 0.25f, 0.25f)); // stretches image to prefered size     
                spriteBatch.Draw(arcade, new Rectangle((int)Window.ClientBounds.Width / 2 - arcade.Width / 2, ((int)Window.ClientBounds.Height / 2 - arcade.Height / 2) + 35, arcade.Width, arcade.Height), null, new Color(0.5f, 0.5f, 0.5f));
                spriteBatch.End();

                GraphicsDevice.Viewport = gameViewport;
                spriteBatch.Begin(SpriteSortMode.Deferred, BlendState.AlphaBlend);
                spriteBatch.Draw(background, new Rectangle(0, 0, 680, 480), null, Color.White); // stretches image to prefered size

                player.Draw(spriteBatch);

                foreach (Laser l in laser)
                    l.Draw(spriteBatch);

                foreach (EnemyLaser l in enemyLaser)
                    l.Draw(spriteBatch);

                foreach (Explosion e in explosion)
                    e.Draw(spriteBatch);

                foreach (Enemy e in enemy)
                    e.Draw(spriteBatch);

                spriteBatch.DrawString(Font1, "Score: " + score.ToString("0000"), new Vector2(640 - Font1.MeasureString("Score: 0000").X - 16, 16), Color.White);

                //spriteBatch.DrawString(Font1, msg, new Vector2(640 / 2 - Font1.MeasureString(msg).X / 2, 480 / 2 - Font1.MeasureString(msg).Y / 2), Color.White);

                if (enemy.Count == 0)
                {
                    gameover = true;
                    spriteBatch.Draw(win_ui, new Rectangle(0, 0, 680, 480), null, Color.White);
                }
                else if (player.dead)
                {
                    gameover = true;
                    spriteBatch.Draw(gameover_ui, new Rectangle(0, 0, 680, 480), null, Color.White);
                }
            }
            else
            {
                spriteBatch.Draw(fmg_logo, Vector2.Zero, null, Color.White);
            }

            spriteBatch.End();

            base.Draw(gameTime);
        }
    }
}
