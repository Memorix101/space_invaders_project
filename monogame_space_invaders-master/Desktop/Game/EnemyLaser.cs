using System;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using Microsoft.Xna.Framework.Content;
using System.Diagnostics;


namespace SpaceInvaders_Desktop
{
    class EnemyLaser
    {
        private Vector2 pos = new Vector2(0, 0);
        private Texture2D tex;
        private float PosX, PosY;
        const int speed = 250;    

        private Rectangle rect;

        public EnemyLaser(Vector2 position, ContentManager con)
        {
            tex = con.Load<Texture2D>("enemy-bullet");
            PosX = position.X;
            PosY = position.Y;
        }

        public virtual Rectangle TileBoundingBox
        {
            get
            {
                return new Rectangle(
                    (int)pos.X,
                    (int)pos.Y,
                    rect.Width,
                    rect.Height);
            }
        }

        public Vector2 Position
        {
            get { return pos; }
        }


        public void Update(GameTime gameTime)
        {
            PosY += 100 * (float)gameTime.ElapsedGameTime.TotalSeconds;
            pos = new Vector2(PosX, PosY);
            
        }

        public void Draw(SpriteBatch sp)
        {
            sp.Draw(tex, pos, Color.White);
        }

    }
}
