package com.mygdx.game;

import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.Input;
import com.badlogic.gdx.graphics.Texture;
import com.badlogic.gdx.graphics.g2d.SpriteBatch;
import com.badlogic.gdx.math.MathUtils;
import com.badlogic.gdx.math.Rectangle;
import com.badlogic.gdx.math.Vector2;

import java.util.ArrayList;

public class Bullet {

    Texture sprite;
    Vector2 pos;
    Rectangle spriteRect = new Rectangle(0, 0, 6, 36);

    boolean revered = false;

    Bullet(Vector2 pos, boolean revered)
    {
        Create(pos, revered);
    }

    public boolean isRevesed()
    {
        return revered;
    }

    public Rectangle TileBoundingBox()
    {
        return new Rectangle(
                pos.x,
                pos.y,
                spriteRect.width,
                spriteRect.height);
    }

    public void Create(Vector2 _pos, boolean revered)
    {
        this.revered = revered;

        if(revered)
        {
            sprite = new Texture("enemy-bullet.png");
        }
        else
        {
            sprite = new Texture("bullet.png");

        }
        spriteRect = new Rectangle(0, 0, sprite.getWidth(), sprite.getHeight());
        pos = new Vector2(_pos.x - sprite.getDepth()/2 + 10, _pos.y + 20);
    }

    public void Update()
    {
        if(!revered)
            pos.y += 350 * Gdx.graphics.getDeltaTime();
        else
            pos.y -= 350 * Gdx.graphics.getDeltaTime();
    }

    public void Draw(SpriteBatch batch)
    {
        batch.draw(sprite, pos.x, pos.y);
    }

}
