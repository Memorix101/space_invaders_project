import framework.Vector2f;
import framework.gfx2D;
import org.lwjgl.*;
import org.lwjgl.glfw.*;
import org.lwjgl.opengl.*;
import org.lwjgl.system.*;

import java.io.IOException;
import java.nio.*;

import static java.awt.Toolkit.getDefaultToolkit;
import static org.lwjgl.glfw.Callbacks.*;
import static org.lwjgl.glfw.GLFW.*;
import static org.lwjgl.opengl.GL11.*;
import static org.lwjgl.stb.STBImage.stbi_load;
import static org.lwjgl.system.MemoryStack.*;
import static org.lwjgl.system.MemoryUtil.*;

public class Game {

    //time stuff
    int start_game = 0; //bool
    float gameSessionTime = 0.f; // time since start
    float Timestep = 0;
    float lastdelta = 0;
    float curTime = 0;

    // The window handle
    private long window;

    public void run() throws IOException {

        System.out.println("LWJGL " + Version.getVersion());

        init();
        loop();

        // Free the window callbacks and destroy the window
        glfwFreeCallbacks(window);
        glfwDestroyWindow(window);

        // Terminate GLFW and free the error callback
        glfwTerminate();
        glfwSetErrorCallback(null).free();
    }

    private void init() {
        // Setup an error callback. The default implementation
        // will print the error message in System.err.
        GLFWErrorCallback.createPrint(System.err).set();

        // Initialize GLFW. Most GLFW functions will not work before doing this.
        if ( !glfwInit() )
            throw new IllegalStateException("Unable to initialize GLFW");

        // Configure GLFW
        glfwDefaultWindowHints(); // optional, the current window hints are already the default
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE); // the window will stay hidden after creation
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // the window will be resizable

        // Create the window
        window = glfwCreateWindow(640, 480, "Space Invaders - LWJGL " + Version.getVersion(), NULL, NULL);
        if ( window == NULL )
            throw new RuntimeException("Failed to create the GLFW window");

        // Setup a key callback. It will be called every time a key is pressed, repeated or released.
        glfwSetKeyCallback(window, (window, key, scancode, action, mods) -> {
            if ( key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE )
                glfwSetWindowShouldClose(window, true); // We will detect this in the rendering loop
        });

        // Get the thread stack and push a new frame
        try ( MemoryStack stack = stackPush() ) {
            IntBuffer pWidth = stack.mallocInt(1); // int*
            IntBuffer pHeight = stack.mallocInt(1); // int*

            // Get the window size passed to glfwCreateWindow
            glfwGetWindowSize(window, pWidth, pHeight);
            setIcon();

            // Get the resolution of the primary monitor
            GLFWVidMode vidmode = glfwGetVideoMode(glfwGetPrimaryMonitor());

            // Center the window
            glfwSetWindowPos(
                    window,
                    (vidmode.width() - pWidth.get(0)) / 2,
                    (vidmode.height() - pHeight.get(0)) / 2
            );
        } // the stack frame is popped automatically

        // Make the OpenGL context current
        glfwMakeContextCurrent(window);
        // Enable v-sync
        glfwSwapInterval(1);

        // Make the window visible
        glfwShowWindow(window);
    }

    private void loop() throws IOException {
        // This line is critical for LWJGL's interoperation with GLFW's
        // OpenGL context, or any context that is managed externally.
        // LWJGL detects the context that is current in the current thread,
        // creates the GLCapabilities instance and makes the OpenGL
        // bindings available for use.
        GL.createCapabilities();

        // init OpenGL
        gfx2D.init();

        Game1.setup(window);
        Game1.init();

        // Set the clear color
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

        // Run the rendering loop until the user has attempted to close
        // the window or has pressed the ESCAPE key.
        while ( !glfwWindowShouldClose(window) ) {

            curTime = (float)glfwGetTime();
            Timestep = curTime - lastdelta;
            lastdelta = curTime;

            Game1.Timestep = Timestep;

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear the framebuffer

            Game1.update();
            Game1.draw();

            glfwSwapBuffers(window); // swap the color buffers

            // Poll for window events. The key callback above will only be
            // invoked during this call.
            glfwPollEvents();
        }
    }

    void setIcon()
    {
        try (MemoryStack stack = MemoryStack.stackPush()) {
            IntBuffer _w = stack.mallocInt(1);
            IntBuffer _h = stack.mallocInt(1);
            IntBuffer comp = stack.mallocInt(1);

            ByteBuffer buffer = stbi_load("res/icon.png", _w, _h, comp, 4);
            GLFWImage image = GLFWImage.malloc();
            image.set(_w.get(), _h.get(), buffer.asReadOnlyBuffer());
            GLFWImage.Buffer images = GLFWImage.malloc(1);
            images.put(0, image);

            glfwSetWindowIcon(window, images);

            images.free();
            image.free();
        }
    }
}