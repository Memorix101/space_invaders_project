package com.libgdx.space_invaders;

import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.graphics.Texture;
import com.badlogic.gdx.graphics.g2d.SpriteBatch;
import com.badlogic.gdx.math.MathUtils;
import com.badlogic.gdx.math.Rectangle;
import com.badlogic.gdx.math.Vector2;

public class Explosion {

    Texture sprite;
    Vector2 pos;
    Rectangle spriteRect = new Rectangle(0, 0, 128, 128);
    Rectangle animationRect;

    private float animationSpeed = 25;
    private float currentFrame;
    public boolean animationCompleted = false;

    Explosion(Vector2 _pos) {
        Create(_pos);
    }

    public Rectangle TileBoundingBox() {
        return new Rectangle(
                pos.x,
                pos.y,
                spriteRect.width,
                spriteRect.height);
    }

    public void Create(Vector2 _pos) {
        sprite = new Texture("explode.png");
        pos = new Vector2(_pos.x, _pos.y);
        spriteRect = new Rectangle(0, 0, sprite.getWidth(), sprite.getHeight());
    }

    private void Animator() {

        currentFrame += animationSpeed * Gdx.graphics.getDeltaTime();
        currentFrame = MathUtils.clamp(currentFrame, 0, 16);

        if (currentFrame >= 16) {
            animationCompleted = true;
        }

        animationRect = new Rectangle((int) currentFrame * 128, 0, 128, 128);
    }


    public void Update() {

        Animator();
        spriteRect = new Rectangle(pos.x, pos.y, 128, 128);
    }

    public void Draw(SpriteBatch batch) {
        batch.draw(sprite, pos.x, pos.y, (int) animationRect.x, (int) animationRect.y, (int) animationRect.width, (int) animationRect.height);
    }
}
