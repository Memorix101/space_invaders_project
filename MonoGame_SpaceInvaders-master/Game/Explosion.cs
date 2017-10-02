using System;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using Microsoft.Xna.Framework.Content;
using System.Diagnostics;

namespace UWP_SpaceInvaders
{
    class Explosion
    {
        private Vector2 pos = new Vector2(0, 0);
        private Texture2D tex;
        private float PosX, PosY;
        private const int moveSpeed = 50;

        private float startPos;

        private Rectangle spriteRect = new Rectangle(0, 0, 128, 128);

        private const float animationSpeed = 25;
        private float currentFrame;

        float spriteWidth;

        public bool animationCompleted = false;

        public Explosion(Vector2 position, ContentManager con)
        {
            startPos = position.X;
            PosX = position.X;
            PosY = position.Y;
            tex = con.Load<Texture2D>("explode");
            spriteWidth = tex.Bounds.Width;
        }

        public Vector2 Position
        {
            get { return pos; }
        }

        private void Animator(GameTime gameTime)
        {
            currentFrame += animationSpeed * (float)gameTime.ElapsedGameTime.TotalSeconds;

            currentFrame = MathHelper.Clamp(currentFrame, 0, 16);

            if (currentFrame >= 16)
            {
                //currentFrame = 0;
                animationCompleted = true;
            }

            spriteRect = new Rectangle((int)currentFrame * 128, 0, 128, 128);
        }

        public void Update(GameTime gameTime)
        {
            Animator(gameTime);

            pos = new Vector2(PosX, PosY);
        }

        public void Draw(SpriteBatch sp)
        {
            sp.Draw(tex, pos, spriteRect, Color.White);
        }
    }
}
