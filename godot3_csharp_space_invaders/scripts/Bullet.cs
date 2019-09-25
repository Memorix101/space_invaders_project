using Godot;
using System;

public class Bullet : Area2D
{
    private float SPEED = -350f;
    private float TIME = 0f;
    private bool HIT = false;

    // Called when the node enters the scene tree for the first time.
    public override void _Ready()
    {
        AddToGroup("entities");
    }

    // Called every frame. 'delta' is the elapsed time since the previous frame.
    public override void _Process(float delta)
    {
        TIME += delta;
        Translate(new Vector2(0, SPEED * delta));
    }

    public override void _PhysicsProcess(float delta)
    {
        if (GetPosition().y <= 0f)
        {
            QueueFree(); //destroys instance
        }
    }

    public void _on_Area2D_body_enter(Area2D other)
    {
        if (HIT == false)
        {
            GD.Print(other.GetName());
            HIT = true;
            other.QueueFree();
            var enemy_pos = other.GetPosition();

            PackedScene go = (PackedScene)ResourceLoader.Load("res://prefabs/explo.tscn"); //bullet prefab
            Sprite explo = (Sprite)go.Instance();

            explo.SetPosition(new Vector2(enemy_pos.x + 75, enemy_pos.y + 75));
            explo.GetChild<AnimationController>(0).Play("anim");
            GetTree().GetRoot().AddChild(explo);
            GetTree().GetRoot().GetNode<GameManager>("GameManager").score += 100;
            GetTree().GetRoot().GetNode<GameManager>("GameManager").killed_enemies += 1;
            QueueFree(); //destroys instance
        }
    }
}
