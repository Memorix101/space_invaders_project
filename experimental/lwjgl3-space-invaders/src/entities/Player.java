package entities;

import framework.Vector2f;
import framework.gfx2D;

import java.io.IOException;

import static org.lwjgl.glfw.GLFW.*;
import static org.lwjgl.glfw.GLFW.GLFW_RELEASE;

public class Player {

    static boolean button_down = false;
    Vector2f p_move = new Vector2f(640 / 2 - 28, 480 - 50);

    public Player() {
    }

    public void input(long w, float Timestep) {
        //continuous-response keys
        if (glfwGetKey(w, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            if (p_move.x < 640 - 28)
                p_move.x += 350 * Timestep;
        }
        if (glfwGetKey(w, GLFW_KEY_LEFT) == GLFW_PRESS) {
            if (p_move.x > 0)
                p_move.x -= 350 * Timestep;
        }

        if (glfwGetKey(w, GLFW_KEY_SPACE) == GLFW_PRESS && button_down == false) {
            // Move up
            button_down = true;
            //printf("BUTTON\n");
            //addBullet(player.pos.x + player.hitbox.w / 2 - 3, player.pos.y);
        } else if (glfwGetKey(w, GLFW_KEY_SPACE) == GLFW_RELEASE && button_down == true) {
            button_down = false;
            //printf("RELEASED\n");
        }
    }

    public void update(float Timestep) {

    }

    public void draw() throws IOException {
        gfx2D.draw("res/player.png", p_move);
    }
}
