using Godot;
using System;

public class Score : Label
{
    // Called when the node enters the scene tree for the first time.
    public override void _Ready()
    {
        
    }

    // Called every frame. 'delta' is the elapsed time since the previous frame.
    public override void _Process(float delta)
    {
        SetText(GetTree().GetRoot().GetNode<GameManager>("GameManager").score.ToString("0000"));
    }
}
