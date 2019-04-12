using System;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using Microsoft.Xna.Framework.Content;
using Microsoft.Xna.Framework.Input;
using System.Diagnostics;

namespace UWP_SpaceInvaders
{
    class Player
    {
        private Vector2 pos;
        private Texture2D tex;
        private const int speed = 250;
        private GamePadState cbuttonState;
        private GamePadState lastcbuttonState;
        private GamePadState cpadState;
        private KeyboardState keyState;

        public bool shoot;
        public bool dead;

        public Player()
        {
            dead = false;
        }

        public virtual Rectangle TileBoundingBox
        {
            get
            {
                return new Rectangle(
                    (int)pos.X,
                    (int)pos.Y,
                    28,
                    21);
            }
        }

        public Vector2 Position
        {
            get { return pos; }
            set { pos = value; }
        }

        public float SpriteWidth
        {
            get { return tex.Bounds.Width; }
        }

        public void LoadResources(ContentManager con)
        {
            tex = con.Load<Texture2D>("player");
            pos = new Vector2(640 / 2 - tex.Bounds.Width / 2, (480 - 60) - tex.Height / 2);
        }

        private void Input(GameTime gameTime)
        {
            cpadState = GamePad.GetState(PlayerIndex.One);
            lastcbuttonState = cbuttonState;

            if (GamePad.GetState(PlayerIndex.One).IsButtonDown(Buttons.A) && lastcbuttonState.IsButtonUp(Buttons.A) ||
                Keyboard.GetState().IsKeyDown(Keys.Space) && keyState.IsKeyDown(Keys.Space) == false)
            {
                shoot = true;
            }
            else
            {
                shoot = false;
            }

            cbuttonState = cpadState;

            if (Keyboard.GetState().IsKeyDown(Keys.D) || Keyboard.GetState().IsKeyDown(Keys.Right) || cpadState.ThumbSticks.Left.X >= 0.5f || cpadState.DPad.Right == ButtonState.Pressed)
            {
                pos.X += speed * (float)gameTime.ElapsedGameTime.TotalSeconds;
            }
            else if (Keyboard.GetState().IsKeyDown(Keys.A) || Keyboard.GetState().IsKeyDown(Keys.Left) || cpadState.ThumbSticks.Left.X <= -0.5f || cpadState.DPad.Left == ButtonState.Pressed)
            {
                pos.X -= speed * (float)gameTime.ElapsedGameTime.TotalSeconds;
            }

            keyState = Keyboard.GetState();
        }

        public void Update(GameTime gameTime)
        {
            if (!dead)
            {
                Input(gameTime);
                pos = new Vector2(MathHelper.Clamp(pos.X, 0, 640 - tex.Width), pos.Y);
                pos = new Vector2(pos.X, (480 - 60) - tex.Height / 2);
            }
        }

        public void Draw(SpriteBatch sp)
        {
            if (!dead)
                sp.Draw(tex, pos, Color.White);
        }
    }
}
