using System;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using Microsoft.Xna.Framework.Content;
using System.Diagnostics;

namespace SpaceInvaders_Desktop
{
    class Enemy
    {
        private Vector2 pos = Vector2.Zero;
        private Texture2D tex;
        private const int moveSpeed = 50;
        private float startPos;
        private Rectangle spriteRect = new Rectangle(0, 0, 32, 32);
        private const float animationSpeed = 25;
        private float currentFrame;
        private bool goLeft = false;
        private int rowPosID;
        private float shootTimer;
        private float shootTimeLimit;

        public bool shoot = false;

        public Enemy(Vector2 position, int rowid, int maxShootTime)
        {
            startPos = position.X;
            pos = new Vector2(position.X, position.Y);
            rowPosID = 40 * (11 - rowid);
            shootTimer = 0f;
            shootTimeLimit = maxShootTime;
        }

        public Vector2 Position
        {
            get { return pos; }
        }

        public virtual Rectangle TileBoundingBox
        {
            get
            {
                return new Rectangle(
                    (int)pos.X,
                    (int)pos.Y,
                    spriteRect.Width,
                    spriteRect.Height);
            }
        }

        private void Animator(GameTime gameTime)
        {
            currentFrame += animationSpeed * (float)gameTime.ElapsedGameTime.TotalSeconds; 
            currentFrame = MathHelper.Clamp(currentFrame, 0, 4);

            if(currentFrame >= 4)
            {
                currentFrame = 0;
            }
            
            spriteRect = new Rectangle((int)currentFrame * 32, 0, 32, 32);
        }

        public void LoadResources(ContentManager con)
        {
            tex = con.Load<Texture2D>("invader32x32x4");
        }

        public void Update(GameTime gameTime)
        {
            Animator(gameTime);

            if (!goLeft)
            {
                pos = new Vector2(pos.X + moveSpeed * (float)gameTime.ElapsedGameTime.TotalSeconds, pos.Y);
            }

            if (goLeft)
            {
                pos = new Vector2(pos.X - moveSpeed * (float)gameTime.ElapsedGameTime.TotalSeconds, pos.Y);
            }

            if (pos.X >= 640 - (spriteRect.Width + rowPosID) && !goLeft)
            {
                goLeft = true;
            }

            if (pos.X <= startPos + spriteRect.Width && goLeft)
            {
                goLeft = false;
            }

            shootTimer += 1 * (float)gameTime.ElapsedGameTime.TotalSeconds;
            
            if (shootTimer >= shootTimeLimit)
            {
                shootTimer = 0f;
                shoot = true;
            }
            else
            {
                shoot = false;
            }
        }

        public void Draw(SpriteBatch sp)
        {
            sp.Draw(tex, pos, spriteRect, Color.White);
        }
    }
}
