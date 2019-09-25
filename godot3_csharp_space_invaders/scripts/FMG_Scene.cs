using Godot;
using System;

public class FMG_Scene : Sprite
{
    private float time = 0f;
    private bool reverse = false;
    private float speed = 0.5f;

    // Called when the node enters the scene tree for the first time.
    public override void _Ready()
    {

    }

    // Called every frame. 'delta' is the elapsed time since the previous frame.
    public override void _Process(float delta)
    {
        //GD.Print("FMG CSharp!");

        this.Modulate = new Color(1f, 1f, 1f, time);

        if (!reverse)
            time += speed * delta;

        if (time >= 1f)
            reverse = true;

        if (reverse)
        {
            time -= speed * delta;
        }

        if (reverse && time <= 0)
            GetTree().ChangeScene("res://scenes/test.tscn");
    }
}