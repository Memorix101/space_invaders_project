using Godot;
using System;

public class GameManager : Node
{
    // GameManager
    public bool done = false;
    public int score = 0;
    public int killed_enemies = 0;
    public bool gameover = false;

    //Scene Script
    private int row = 0;
    private int rowCount = 0;
    private int lastRand = 0;

    // Called when the node enters the scene tree for the first time.
    public override void _Ready()
    {
        Random r = new Random();

        for (int i = 0; i < 40; i++)
        {
            //GD.Print("Hi " + i);
            rowCount += 1;

            if (rowCount == 9)
            {
                rowCount = 1;
                row += 1;
            }

            PackedScene go = (PackedScene)ResourceLoader.Load("res://prefabs/enemy.tscn"); // prefab
            KinematicBody2D enemy = (KinematicBody2D)go.Instance();
            enemy.SetPosition(new Vector2(64 * rowCount, 100 + (50 * row)));

            int rnd = r.Next(3, 20);

            /*if (lastRand != rnd)
            {
                GD.Print("same rnd");
                rnd = r.Next(5, 20);
            }*/

            enemy.GetNode<Enemy>(".").setFireRate(rnd); //seconds
            lastRand = rnd;

            AddChild(enemy);
            done = true;
        }
    }

    // Called every frame. 'delta' is the elapsed time since the previous frame.
    public override void _Process(float delta)
    {
        if (killed_enemies >= 40)
        {
            gameover = true;
        }

        if (killed_enemies >= 40 && gameover == true)
        {
            GetNode<Sprite>("win_ui").Show();
        }

        if (killed_enemies < 40 && gameover == true)
        {
            GetNode<Sprite>("gameover_ui").Show();
        }

        if (Input.IsActionPressed("quit_game"))
        {
            GetTree().Quit(); // default behavior
        }

        if (gameover == true && Input.IsActionPressed("ui_accept"))
        {
            GetNode<Sprite>("gameover_ui").Hide();
            GetNode<Sprite>("win_ui").Hide();
            score = 0;
            killed_enemies = 0;
            gameover = false;
            // remove all active nodes
            RestartGame();
        }
    }

    public void RestartGame()
    {
        GD.Print("restarting level ... ");
        foreach (Node go in GetTree().GetNodesInGroup("entities"))
        {
            go.QueueFree();
        }

        GetTree().ChangeScene("res://scenes/test.tscn");
    }
}
