package com.mygdx.game;

import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.Input;
import com.badlogic.gdx.graphics.Texture;
import com.badlogic.gdx.graphics.g2d.SpriteBatch;
import com.badlogic.gdx.math.MathUtils;
import com.badlogic.gdx.math.Rectangle;
import com.badlogic.gdx.math.Vector2;

public class Player {

    Texture sprite;
    Vector2 pos;
    Rectangle spriteRect;

    public boolean dead = false;

    Player()
    { }

    public Rectangle TileBoundingBox()
    {
        return new Rectangle(
                pos.x,
                pos.y,
                spriteRect.width,
                spriteRect.height);
    }

    public void Create()
    {
        sprite = new Texture("player.png");
        pos = new Vector2(640/2 - sprite.getDepth()/2,10);
        spriteRect = new Rectangle(0, 0, sprite.getWidth(), sprite.getHeight());
    }

    public void Input()
    {
        if(Gdx.input.isKeyPressed(Input.Keys.A) || Gdx.input.isKeyPressed(Input.Keys.LEFT))
        {
            pos.x -= 350 * Gdx.graphics.getDeltaTime();
        }

        if(Gdx.input.isKeyPressed(Input.Keys.D) || Gdx.input.isKeyPressed(Input.Keys.RIGHT))
        {
            pos.x += 350 * Gdx.graphics.getDeltaTime();;
        }

        pos.x = MathUtils.clamp(pos.x, 0f, 640f - 32f);
    }
    
    public void Update()
    {
        Input();
    }

    public void Draw(SpriteBatch batch)
    {
        if(!dead)
            batch.draw(sprite, pos.x, pos.y);
    }
}
