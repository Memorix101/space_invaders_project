using Godot;
using System;

public class Player : KinematicBody2D
{
    private float SPEED = 350f;
    private bool BUTTON_ACTIVE = false;

    // Called when the node enters the scene tree for the first time.
    public override void _Ready()
    {

    }

    // Called every frame. 'delta' is the elapsed time since the previous frame.
    public override void _Process(float delta)
    {
        var player_pos = GetPosition();
        //limit movement axis
        player_pos.x = Mathf.Clamp(player_pos.x, 30, 995);
        //shooting lasers
        if (Input.IsActionPressed("fire") && BUTTON_ACTIVE == false)
        { 
            BUTTON_ACTIVE = true;

            //bullet prefab
            PackedScene go = (PackedScene)ResourceLoader.Load("res://prefabs/bullet.tscn");
            Area2D bullet = (Area2D)go.Instance();
            //AddChild(bullet);

            bullet.SetPosition(new Vector2(player_pos.x + 85, player_pos.y - 5));

            GetTree().GetRoot().AddChild(bullet);
        }
        else if (!Input.IsActionPressed("fire") && BUTTON_ACTIVE)
        {
            BUTTON_ACTIVE = false;
        }
        
        //moving player
        if (Input.IsActionPressed("ui_left"))
        {
            player_pos.x -= delta * SPEED;
        }
        else if (Input.IsActionPressed("ui_right"))
        {
            player_pos.x += delta * SPEED;
        }

        //update pos
        SetPosition(player_pos);
    }
}
