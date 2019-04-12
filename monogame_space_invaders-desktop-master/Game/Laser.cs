using System;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using Microsoft.Xna.Framework.Content;
using System.Diagnostics;


namespace SpaceInvaders_Desktop
{
    class Laser
    {
        private Vector2 pos = new Vector2(0, 0);
        private Texture2D tex;
        private const int speed = 250;

        public bool isEnemy = false;
        public bool dispose = false;

        public Laser(Vector2 position, bool isEnemy, ContentManager con)
        {
            pos.X = position.X;
            pos.Y = position.Y;
            this.isEnemy = isEnemy;

            if(isEnemy)
                tex = con.Load<Texture2D>("enemy-bullet");
            else
                tex = con.Load<Texture2D>("bullet");
        }

        public virtual Rectangle TileBoundingBox
        {
            get
            {
                return new Rectangle(
                    (int)pos.X,
                    (int)pos.Y,
                    tex.Width,
                    tex.Height);
            }
        }

        public Vector2 Position
        {
            get { return pos; }
        }

        public void Update(GameTime gameTime)
        {
            if (isEnemy)
                pos = new Vector2(pos.X, pos.Y + speed * (float)gameTime.ElapsedGameTime.TotalSeconds);
            else
                pos = new Vector2(pos.X, pos.Y - speed * (float)gameTime.ElapsedGameTime.TotalSeconds);
        }

        public void Draw(SpriteBatch sp)
        {
            sp.Draw(tex, pos, Color.White);
        }
    }
}
