package framework;

import java.nio.IntBuffer;

import org.lwjgl.system.MemoryStack;

import java.io.IOException;
import java.nio.ByteBuffer;

import static org.lwjgl.opengl.GL20.*;
import static org.lwjgl.stb.STBImage.*;

public class gfx2D {

    public static void init()
    {
        // init OpenGL
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, 640, 480, 0, 1, -1);
        glMatrixMode(GL_MODELVIEW);
    }

    public static void draw(String imagePath, Vector2f vec) throws IOException {
        try (MemoryStack stack = MemoryStack.stackPush()) {

            /*// https://stackoverflow.com/questions/41901468/load-a-texture-within-opengl-and-lwjgl3-in-java
            //load png file
            PNGDecoder decoder = new PNGDecoder(gfx2d.class.getResourceAsStream(imagePath));
            //create a byte buffer big enough to store RGBA values
            ByteBuffer buffer = ByteBuffer.allocateDirect(4 * decoder.getWidth() * decoder.getHeight());
            //decode
            decoder.decode(buffer, decoder.getWidth() * 4, PNGDecoder.Format.RGBA);

            //flip the buffer so its ready to read
            buffer.flip();*/

            /* Load image */
            ByteBuffer buffer;
            int width, height;

            /* Prepare image buffers */
            IntBuffer _w = stack.mallocInt(1);
            IntBuffer _h = stack.mallocInt(1);
            IntBuffer comp = stack.mallocInt(1);
            stbi_set_flip_vertically_on_load(false);
            buffer = stbi_load(imagePath, _w, _h, comp, 4);

            if (buffer == null) {
                throw new RuntimeException("Failed to load a texture file!"
                        + System.lineSeparator() + stbi_failure_reason());
            }

            /* Get width and height of image */
            width = _w.get();
            height = _h.get();

            //create a texture
            int id = glGenTextures();
            glBindTexture(GL_TEXTURE_2D, id);
            //tell opengl how to unpack bytes
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            //upload texture
            //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, decoder.getWidth(), decoder.getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);

            glEnable(GL_TEXTURE_2D);

            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

            /*IntBuffer w = stack.mallocInt(1), h = stack.mallocInt(1);
            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, w);
            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, h);*/

            /*int w = decoder.getWidth();
            int h = decoder.getHeight();*/

            int w = width, h = height;

            glPushMatrix();

            glBegin(GL_QUADS);

            glTexCoord2f(0.0f, 0.0f);
            glVertex2f(vec.x, vec.y); //100 100 // 1 - lead vec pos

            glTexCoord2f(1.0f, 0.0f);
            glVertex2f(vec.x + w, vec.y); //300 100 // 2

            glTexCoord2f(1.0f, 1.0f);
            glVertex2f(vec.x + w, vec.y + h); //300 300 // 3

            glTexCoord2f(0.0f, 1.0f);
            glVertex2f(vec.x, vec.y + h); //100 300  // 4

            glEnd();

            glPopMatrix();
            glDisable(GL_TEXTURE_2D);
        }
        catch (Exception ex)
        {
            System.out.println(ex);
        }
    }
}
