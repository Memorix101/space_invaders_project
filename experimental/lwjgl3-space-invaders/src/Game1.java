
import entities.Player;
import framework.Vector2f;
import framework.gfx2D;

import java.io.IOException;

import static org.lwjgl.glfw.GLFW.*;

public class Game1 {

    static long _window;
    public static float Timestep;

    static Player _player = new Player();

    public static void setup(long window)
    {
        _window = window;
    }

    public static void init()
    {

    }

    public static void update()
    {
        _player.input(_window, Timestep);
        _player.update(Timestep);
    }

    public static void draw() throws IOException {
        gfx2D.draw("res/space3.png", new Vector2f(0, 0));
        _player.draw();
    }
}
