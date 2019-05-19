package com.libgdx.space_invaders;

import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.Input;
import com.badlogic.gdx.controllers.Controller;
import com.badlogic.gdx.controllers.PovDirection;
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

    void ControllerInput(Controller gpad)
    {
        if(gpad.getAxis(1) <= -0.5f || gpad.getPov(0)== PovDirection.west)
        {
            pos.x -= 350 * Gdx.graphics.getDeltaTime();
        }

        if(gpad.getAxis(1) >= 0.5f || gpad.getPov(0) == PovDirection.east)
        {
            pos.x += 350 * Gdx.graphics.getDeltaTime();;
        }
    }

    public void Input(Controller gpad)
    {
        if(gpad != null)
            ControllerInput(gpad);

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

    }

    public void Draw(SpriteBatch batch)
    {
        if(!dead)
            batch.draw(sprite, pos.x, pos.y);
    }
}
