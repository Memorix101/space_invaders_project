package com.libgdx.space_invaders;

import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.Input;
import com.badlogic.gdx.graphics.Texture;
import com.badlogic.gdx.graphics.g2d.SpriteBatch;
import com.badlogic.gdx.math.MathUtils;
import com.badlogic.gdx.math.Rectangle;
import com.badlogic.gdx.math.Vector2;

public class Enemy {

    Texture sprite;
    Vector2 pos;
    Rectangle spriteRect = new Rectangle(0, 0, 32, 32);
    Rectangle animationRect;

    public boolean shoot = false;

    private float animationSpeed = 25;
    private float currentFrame;

    private boolean goLeft = false;
    private int rowPosID;
    private float shootTimer;
    private float startPos;
    private int moveSpeed = 50;
    private float shootTimeLimit;

    Enemy(Vector2 _pos, int rowid, int maxShootTime) {
        Create(_pos, rowid, maxShootTime);
    }

    public Rectangle TileBoundingBox() {
        return new Rectangle(
                pos.x,
                pos.y,
                spriteRect.width,
                spriteRect.height);
    }

    public void Create(Vector2 _pos, int rowid, int maxShootTime) {
        sprite = new Texture("invader.png");
        spriteRect = new Rectangle(0, 0, sprite.getWidth(), sprite.getHeight());

        startPos = _pos.x;
        pos = new Vector2(_pos.x, _pos.y);
        rowPosID = 40 * (11 - rowid);
        shootTimer = 0f;
        shootTimeLimit = maxShootTime;
    }

    private void Animator() {
        currentFrame += animationSpeed * Gdx.graphics.getDeltaTime();
        currentFrame = MathUtils.clamp(currentFrame, 0, 4);

        if (currentFrame >= 4) {
            currentFrame = 0;
        }
        animationRect = new Rectangle((int) currentFrame * 32, 0, 32, 32);
    }


    public void Update() {

        Animator();

        if (!goLeft)
        {
            pos.x += moveSpeed * Gdx.graphics.getDeltaTime();
        }

        if (goLeft)
        {
            pos.x -= moveSpeed * Gdx.graphics.getDeltaTime();
        }

        if (pos.x >= 640 - (spriteRect.width + rowPosID) && !goLeft)
        {
            goLeft = true;
        }

        if (pos.x <= startPos + spriteRect.width && goLeft)
        {
            goLeft = false;
        }

        shootTimer += 1 * Gdx.graphics.getDeltaTime();

        if (shootTimer >= shootTimeLimit)
        {
            shootTimer = 0f;
            shoot = true;
        }
        else
        {
            shoot = false;
        }

        spriteRect = new Rectangle(pos.x, pos.y, 32, 32);
    }

    public void Draw(SpriteBatch batch) {
        batch.draw(sprite, pos.x, pos.y, (int) animationRect.x, (int) animationRect.y, (int) animationRect.width, (int) animationRect.height);
    }
}
