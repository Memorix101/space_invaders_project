using System;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Audio;
using Microsoft.Xna.Framework.Graphics;
using Microsoft.Xna.Framework.Media;
using System.Collections.Generic;
using System.Diagnostics;
using Windows.UI.ViewManagement;
using Microsoft.Xna.Framework.Input;
using Windows.Graphics.Display;

namespace UWP_SpaceInvaders
{
    public class Game1 : Game
    {
        // MonoGame
        private GraphicsDeviceManager graphics;
        private SpriteBatch spriteBatch;

        //UWP
        private float screenWidth;
        private float screenHeight;

        // Sprites
        private Texture2D fmg_logo;
        private Texture2D background;
        private Texture2D arcade;
        private Texture2D gameover_ui;
        private Texture2D win_ui;

        // Entities
        private Player player;
        private List<Laser> laser;
        private List<Laser> enemyLaser;
        private List<Explosion> explosion;
        private List<Enemy> enemy;

        // Enemy Creation
        private int rowCount;
        private int itemCount;

        // GameManager
        private bool gamestart = false;
        private float timer = 0f;
        private bool gameover = false;

        // Font
        private SpriteFont verminFont;
        private string msg = string.Empty;
        private int score;

        // Audio
        private Song music;
        private SoundEffect snd_blaster;
        private SoundEffect snd_blasterEnemy;
        private SoundEffect snd_explo;

        public Game1()
        {
            graphics = new GraphicsDeviceManager(this);
            Content.RootDirectory = "Content";
            graphics.PreferredBackBufferHeight = 1080;
            graphics.PreferredBackBufferWidth = 1920;
        }

        private void Restart()
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
            SpawnEnemies();
            player.dead = false;
            gameover = false;
            gamestart = true;
        }

        private void SpawnEnemies()
        {
            Random r = new Random();

            //Create enemies
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
            ApplicationView.PreferredLaunchWindowingMode = ApplicationViewWindowingMode.FullScreen;
            screenHeight = (float)ApplicationView.GetForCurrentView().VisibleBounds.Height;
            screenWidth = (float)ApplicationView.GetForCurrentView().VisibleBounds.Width;
            this.IsMouseVisible = false;

            player = new Player();
            enemy = new List<Enemy>();
            laser = new List<Laser>();
            enemyLaser = new List<Laser>();
            explosion = new List<Explosion>();

            // Create enemies
            SpawnEnemies();

            base.Initialize();
        }

        protected override void LoadContent()
        {
            // Create a new SpriteBatch, which can be used to draw textures.
            spriteBatch = new SpriteBatch(GraphicsDevice);

            player.LoadResources(Content);
            fmg_logo = Content.Load<Texture2D>("fmg_15b");
            background = Content.Load<Texture2D>("space3");
            arcade = Content.Load<Texture2D>("arcade");
            gameover_ui = Content.Load<Texture2D>("gameover_ui");
            win_ui = Content.Load<Texture2D>("win_ui");
            verminFont = Content.Load<SpriteFont>("font");

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
            if (GamePad.GetState(PlayerIndex.One).IsButtonDown(Buttons.Back) || Keyboard.GetState().IsKeyDown(Keys.Escape))
                Exit();

            timer += 1 * (float)gameTime.ElapsedGameTime.TotalSeconds;

            if (timer >= 3f)
                gamestart = true;

            if (gamestart)
            {
                player.Update(gameTime);

                if (player.shoot)
                {
                    Debug.WriteLine("SHOOT");
                    laser.Add(new Laser(new Vector2(player.Position.X + player.SpriteWidth / 2 - 1.5f, player.Position.Y - 32), false, Content)); //1.5f is half of laser width ;)
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
                        enemyLaser.Add(new Laser(new Vector2(enemy[e].Position.X + 32 / 2 - 4.5f, enemy[e].Position.Y + 9), true, Content));
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

                if (GamePad.GetState(PlayerIndex.One).IsButtonDown(Buttons.Start) && gameover ||
                    Keyboard.GetState().IsKeyDown(Keys.Enter) && gameover)
                {
                    Restart();
                }
            }

            base.Update(gameTime);
        }

        protected override void Draw(GameTime gameTime)
        {
            GraphicsDevice.Clear(Color.Black);
            
            if (gamestart)
            {
                spriteBatch.Begin(SpriteSortMode.Deferred, BlendState.AlphaBlend);

                GraphicsDevice.Viewport = new Viewport((int)screenWidth / 2 - 640 / 2,
                    (int)screenHeight / 2 - 480 / 2 - 35, 640, 480);

                spriteBatch.Draw(background, new Rectangle(0, 0, 680, 480), null, Color.White); // stretches image to preferred size

                player.Draw(spriteBatch);

                foreach (Laser l in laser)
                    l.Draw(spriteBatch);

                foreach (Laser l in enemyLaser)
                    l.Draw(spriteBatch);

                foreach (Explosion e in explosion)
                    e.Draw(spriteBatch);

                foreach (Enemy e in enemy)
                    e.Draw(spriteBatch);

                spriteBatch.DrawString(verminFont, "Score: " + score.ToString("0000"),
                    new Vector2(640 - verminFont.MeasureString("Score: 0000").X - 16, 16), Color.White);
                //spriteBatch.DrawString(verminFont, msg, new Vector2(640 / 2 - verminFont.MeasureString(msg).X / 2,
                //480 / 2 - verminFont.MeasureString(msg).Y / 2), Color.White);

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
                spriteBatch.End();
            }
            else
            {
                GraphicsDevice.Viewport = new Viewport((int)screenWidth / 2 - 640 / 2,
                    (int)screenHeight / 2 - 480 / 2 - 35, 640, 480);

                spriteBatch.Begin(SpriteSortMode.Deferred, BlendState.AlphaBlend);
                spriteBatch.Draw(fmg_logo, Vector2.Zero, new Rectangle(0, 0, 680, 480), Color.White);
                spriteBatch.End();
            }

            GraphicsDevice.Viewport = new Viewport(0, 0, (int)screenWidth, (int)screenHeight);
            spriteBatch.Begin(SpriteSortMode.Deferred, BlendState.AlphaBlend);
            //spriteBatch.Draw(frameBackground, new Rectangle(0, 0, GraphicsDevice.Viewport.Width, GraphicsDevice.Viewport.Height),
            //null, new Color(0.25f, 0.25f, 0.25f)); // stretches image to prefered size     
            spriteBatch.Draw(arcade, new Rectangle((int)screenWidth / 2 - arcade.Width / 2,
                ((int)screenHeight / 2 - arcade.Height / 2), arcade.Width, arcade.Height), null, Color.White);
            spriteBatch.End();

            base.Draw(gameTime);
        }
    }
}
