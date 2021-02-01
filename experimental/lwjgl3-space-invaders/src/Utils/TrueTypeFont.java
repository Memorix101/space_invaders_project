package Utils;

import java.nio.ByteBuffer;

import Utils.IOUtil;
import org.lwjgl.BufferUtils;
import org.lwjgl.opengl.GL11;
import org.lwjgl.stb.STBTTBakedChar;

public class TrueTypeFont {
    private static final int BITMAP_W = 512;
    private static final int BITMAP_H = 512;

    private int size;

    public TrueTypeFont(int size) {
        this.size = size;

        int texID = GL11.glGenTextures();
        STBTTBakedChar.Buffer cdata = STBTTBakedChar.malloc(96);

        try {
            ByteBuffer ttf = IOUtil.ioResourceToByteBuffer("Resources/temp/Arial.ttf", 160 * 1024);
            if ( ttf == null ) {
                System.err.println("Font not found");
                return;
            }

            ByteBuffer bitmap = BufferUtils.createByteBuffer(BITMAP_W * BITMAP_H);
            org.lwjgl.stb.STBTruetype.stbtt_BakeFontBitmap(ttf, getFontHeight(), bitmap, BITMAP_W, BITMAP_H, 32, cdata);


            GL11.glBindTexture(GL11.GL_TEXTURE_2D, texID);
            GL11.glTexImage2D(GL11.GL_TEXTURE_2D, 0, GL11.GL_ALPHA, BITMAP_W, BITMAP_H, 0, GL11.GL_ALPHA, GL11.GL_UNSIGNED_BYTE, bitmap);
            GL11.glTexParameteri(GL11.GL_TEXTURE_2D, GL11.GL_TEXTURE_MAG_FILTER, GL11.GL_LINEAR);
            GL11.glTexParameteri(GL11.GL_TEXTURE_2D, GL11.GL_TEXTURE_MIN_FILTER, GL11.GL_LINEAR);

            org.lwjgl.stb.STBImageWrite.stbi_write_png("Resources/test.png", BITMAP_W, BITMAP_H, 1, bitmap, 4);

        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public int getFontHeight() {
        return size;
    }
}