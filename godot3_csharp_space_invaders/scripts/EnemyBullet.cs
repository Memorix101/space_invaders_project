using Godot;
using System;

public class EnemyBullet : Area2D
{
    private float SPEED = 350f;
    private float TIME = 0f;

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
        if (GetPosition().y >= 600)
        {
            QueueFree(); //destroys instance
        }
    }

    public void _on_Area2D_body_enter(Area2D other)
    {
        if (other.GetName() == "Player")
        {
            PackedScene go = (PackedScene)ResourceLoader.Load("res://prefabs/explo.tscn"); //explo prefab
            Sprite explo = (Sprite)go.Instance();

            explo.SetPosition(new Vector2(other.GetPosition().x + 100, other.GetPosition().y + 24));
            explo.GetChild<AnimationController>(0).Play("anim");
            GetTree().GetRoot().AddChild(explo);

            other.QueueFree();
            QueueFree(); // destroys instance

            //CallDeferred("queue_free()");	

            GetTree().GetRoot().GetNode<GameManager>("GameManager").gameover = true;
        }
    }
}
