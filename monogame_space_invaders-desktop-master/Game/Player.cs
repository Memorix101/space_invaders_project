using System;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using Microsoft.Xna.Framework.Content;
using Microsoft.Xna.Framework.Input;
using System.Diagnostics;

namespace SpaceInvaders_Desktop
{
    class Player
    {
        Vector2 pos;
        private Texture2D tex;
        const int speed = 250;

        public bool shoot;

        float spriteWidth;

        GamePadState keyState;
        GamePadState lastkeyState;
        GamePadState gamePadState;

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
            get { return spriteWidth; }
        }

        public void LoadResources(ContentManager con)
        {
            tex = con.Load<Texture2D>("player");
            pos = new Vector2(640 / 2 - tex.Bounds.Width / 2, (480 - 60) - tex.Height / 2);
            spriteWidth = tex.Bounds.Width;
        }

        private void Input(GameTime gameTime)
        {
            gamePadState = GamePad.GetState(PlayerIndex.One);
            lastkeyState = keyState;

            if (GamePad.GetState(PlayerIndex.One).IsButtonDown(Buttons.A) && lastkeyState.IsButtonUp(Buttons.A))
            {
                shoot = true;
            }
            else
            {
                shoot = false;
            }

            keyState = gamePadState;

            if (gamePadState.ThumbSticks.Left.X >= 0.5f)
            {
                pos.X += speed * (float)gameTime.ElapsedGameTime.TotalSeconds;
            }
            else if (gamePadState.ThumbSticks.Left.X <= -0.5f)
            {
                pos.X -= speed * (float)gameTime.ElapsedGameTime.TotalSeconds; 
            }

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
            if(!dead)
            sp.Draw(tex, pos, Color.White);
        }
    }
}
