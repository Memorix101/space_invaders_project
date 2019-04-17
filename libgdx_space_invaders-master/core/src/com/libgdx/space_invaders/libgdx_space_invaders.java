package com.libgdx.space_invaders;

import com.badlogic.gdx.ApplicationAdapter;
import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.Input;
import com.badlogic.gdx.audio.Music;
import com.badlogic.gdx.audio.Sound;
import com.badlogic.gdx.controllers.Controller;
import com.badlogic.gdx.controllers.Controllers;
import com.badlogic.gdx.graphics.GL20;
import com.badlogic.gdx.graphics.Texture;
import com.badlogic.gdx.graphics.g2d.BitmapFont;
import com.badlogic.gdx.graphics.g2d.SpriteBatch;
import com.badlogic.gdx.graphics.g2d.freetype.FreeTypeFontGenerator;
import com.badlogic.gdx.math.Vector2;

import java.util.ArrayList;
import java.util.Random;

public class libgdx_space_invaders extends ApplicationAdapter {

    SpriteBatch batch;
    Texture background_sprite;
    Texture fmg_logo;
    Texture gameover_ui;
    Texture win_ui;
    BitmapFont score_font;

    int score = 0;
    int rowCount;
    int itemCount;

    float timer;
    boolean game_start = false;
    boolean gameover = false;
    boolean key_firePressed = false;
    boolean gpad_firePressed = false;

    Music music;
    Sound snd_blaster;
    Sound snd_blasterEnemy;
    Sound snd_explo;

    Player player = new Player();
    ArrayList<Bullet> laser = new ArrayList<Bullet>();
    ArrayList<Bullet> pusher = new ArrayList<Bullet>();
    ArrayList<Enemy> enemies = new ArrayList<Enemy>();
    ArrayList<Explosion> explode_fx = new ArrayList<Explosion>();

    Controller gpad;

    void restart() {
        player.dead = false;
        player.pos = new Vector2(640 / 2 - player.sprite.getDepth() / 2, 10);
        laser.clear();
        enemies.clear();
        pusher.clear();
        explode_fx.clear();
        itemCount = 0;
        rowCount = 0;
        score = 0;
        gameover = false;
        spawnEnemies();
    }

    void spawnEnemies() {
        // create enemies
        for (int i = 0; i < 40; i++) {
            if (i % 10 == 0) {
                itemCount = 0;
                rowCount++;
            }

            itemCount++;

            Random r = new Random();
            int ran = r.nextInt(30 - 3) + 1;
            enemies.add(new Enemy(new Vector2(itemCount * 40, 40 * rowCount + 200), itemCount, ran));
        }
    }

    @Override
    public void create() {

        for (Controller controller : Controllers.getControllers()) {
            Gdx.app.log("Conntected: ", controller.getName());
            gpad = Controllers.getControllers().first();
        }

        FreeTypeFontGenerator generator = new FreeTypeFontGenerator(Gdx.files.internal("vermin_vibes_1989.ttf"));
        FreeTypeFontGenerator.FreeTypeFontParameter parameter = new FreeTypeFontGenerator.FreeTypeFontParameter();
        parameter.size = 32;
        score_font = generator.generateFont(parameter);
        generator.dispose(); // don't forget to dispose to avoid memory leaks!

        batch = new SpriteBatch();
        background_sprite = new Texture("space3.png");
        fmg_logo = new Texture("fmg_splash.png");
        gameover_ui = new Texture("gameover_ui.psd");
        win_ui = new Texture("win_ui.psd");

        player.Create();

        spawnEnemies();

        music = Gdx.audio.newMusic(Gdx.files.internal("bodenstaendig.ogg"));
        music.setLooping(true);
        music.play();

        snd_blaster = Gdx.audio.newSound(Gdx.files.internal("blaster.ogg"));
        snd_blasterEnemy = Gdx.audio.newSound(Gdx.files.internal("pusher.ogg"));
        snd_explo = Gdx.audio.newSound(Gdx.files.internal("explode1.ogg"));
    }

    void ControllerInput()
    {
        if (gpad.getButton(0) && !player.dead)
        { //https://github.com/libgdx/libgdx/issues/2200#issuecomment-51062919
            if (!gpad_firePressed){
                gpad_firePressed = true;
                laser.add(new Bullet(player.pos, false));
                snd_blaster.play();
            }
        } else {
            gpad_firePressed = false;
        }

        if (gpad.getButton(7) && gameover) {
            restart();
        }

        if (gpad.getButton(6)) {
            Gdx.app.exit();
        }
    }

    void Update() {

        timer += 1 * Gdx.graphics.getDeltaTime();

        if (timer >= 3f)
            game_start = true;

        if (game_start) {
            player.Update();
            player.Input(gpad);

            if (Gdx.input.isKeyPressed(Input.Keys.SPACE) && !player.dead)
            { //https://github.com/libgdx/libgdx/issues/2200#issuecomment-51062919
                if (!key_firePressed)
                {
                    key_firePressed = true;
                    laser.add(new Bullet(player.pos, false));
                    snd_blaster.play();
                }
            } else {
                key_firePressed = false;
            }

            if (Gdx.input.isKeyJustPressed(Input.Keys.ENTER) && gameover) {
                restart();
            }

            if (Gdx.input.isKeyJustPressed(Input.Keys.ESCAPE)) {
                Gdx.app.exit();
            }

            if (gpad != null)
                ControllerInput();

            for (int e = enemies.size() - 1; e >= 0; e--) {
                enemies.get(e).Update();

                if (enemies.get(e).shoot) {
                    pusher.add(new Bullet(new Vector2(enemies.get(e).pos.x + 32 / 2 - 15, enemies.get(e).pos.y - 9), true));
                    snd_blasterEnemy.play();
                }

                for (int l = laser.size() - 1; l >= 0; l--) {
                    if (laser.get(l).TileBoundingBox().overlaps(enemies.get(e).spriteRect)) {
                        score += 100;
                        explode_fx.add(new Explosion(new Vector2(laser.get(l).pos.x - 128 * 0.475f, laser.get(l).pos.y - 128 * 0.2f)));
                        laser.remove(l);
                        enemies.remove(e);
                        snd_explo.play();
                        break;
                    }
                }
            }

            for (Bullet b : laser) {
                b.Update();
            }

            for (int l = laser.size() - 1; l >= 0; l--) {
                if (laser.get(l).pos.y >= 480)
                    laser.remove(l);
            }

            for (int p = pusher.size() - 1; p >= 0; p--) {
                pusher.get(p).Update();

                if (pusher.get(p).pos.y >= 480)
                    pusher.remove(p);

                if (pusher.get(p).TileBoundingBox().overlaps(player.TileBoundingBox()) && !player.dead) {
                    pusher.remove(p);
                    explode_fx.add(new Explosion(new Vector2(player.pos.x - 128 * 0.4f, player.pos.y - 128 / 2)));
                    player.dead = true;
                    snd_explo.play();
                }
            }

            for (int e = explode_fx.size() - 1; e >= 0; e--) {
                explode_fx.get(e).Update();
                if (explode_fx.get(e).animationCompleted)
                    explode_fx.remove(e);
            }
        }
    }

    @Override
    public void render() {
        Update();

        Gdx.gl.glClearColor(0, 0, 0, 1);
        Gdx.gl.glClear(GL20.GL_COLOR_BUFFER_BIT);
        batch.begin();

        if (game_start) {
            batch.draw(background_sprite, 0, -120);

            for (Bullet b : laser)
                b.Draw(batch);

            for (Bullet p : pusher)
                p.Draw(batch);

            for (Explosion e : explode_fx)
                e.Draw(batch);

            for (Enemy e : enemies)
                e.Draw(batch);

            player.Draw(batch);
            score_font.draw(batch, "SCORE: " + String.format("%04d", score), 640 - 190, 480 - 10);

            if (score >= 4000) {
                //score_font.draw(batch, "YOU WIN!", 640 / 2 - 35, 480 / 2 - 10);
                batch.draw(win_ui, 0, 0);
                gameover = true;
            }

            if (player.dead) {
                //score_font.draw(batch, "GAME OVER!", 640 / 2 - 70, 480 / 2 - 10);
                batch.draw(gameover_ui, 0, 0);
                gameover = true;
            }
        } else {
            batch.draw(fmg_logo, 0, 0);
        }
        batch.end();
    }

    @Override
    public void dispose() {
        batch.dispose();
        background_sprite.dispose();
    }
}
