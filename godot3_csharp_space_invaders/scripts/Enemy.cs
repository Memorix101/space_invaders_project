using Godot;
using System;

public class Enemy : KinematicBody2D
{
    private float SPEED = 250f;
    private float TIME = 0f;
    private float START_POS = 0f;
    private bool move_left = false;
    private bool move_right = true;
    private int fire_rate = 0;
    private float fire_timer = 0f;

    // Called when the node enters the scene tree for the first time.
    public override void _Ready()
    {
        GD.Print(fire_rate);
        START_POS = GetPosition().x;
        AddToGroup("entities");
    }

    // Called every frame. 'delta' is the elapsed time since the previous frame.
    public override void _Process(float delta)
    {
        Vector2 enemy_pos = GetPosition();
        fire_timer += delta;

        if (fire_timer >= fire_rate)
        {
            // spawn bullet
            PackedScene go = (PackedScene)ResourceLoader.Load("res://prefabs/enemy_bullet.tscn"); // prefab
            Area2D enemy_bullet = (Area2D)go.Instance();
            enemy_bullet.SetPosition(new Vector2(enemy_pos.x, enemy_pos.y + 50));
            GetTree().GetRoot().AddChild(enemy_bullet);

            // reset
            fire_timer = 0;
        }

        if (GetPosition().x <= START_POS && move_left)
        {
            // move right
            move_right = true;
            move_left = false;
            SPEED = SPEED * -1;
        }
        else if (GetPosition().x >= START_POS + 500 && move_right)
        {
            // move left
            move_right = false;
            move_left = true;
            SPEED = SPEED * -1;
        }


        if (GetNode<GameManager>("..").done)
        {
            Translate(new Vector2(SPEED * delta, 0));
        }
    }

    public void setFireRate(int v)
    {
        fire_rate = v;
    }
}
