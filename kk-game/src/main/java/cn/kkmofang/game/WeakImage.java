package cn.kkmofang.game;

import android.graphics.Bitmap;

import java.nio.ByteBuffer;

import cn.kkmofang.view.value.Pixel;

/**
 * Created by hailong11 on 2018/6/25.
 */

public class WeakImage extends WeakTexture {


    public final static int STATUS_NONE = 0;
    public final static int STATUS_LOADED = 1;
    public final static int STATUS_LOADING = 2;
    public final static int STATUS_FAIL = 3;

    public void setStatus(int status,String errmsg) {
        setStatus(_ptr,status,errmsg);
    }

    public void setBitmap(Bitmap bitmap) {

        int width = bitmap.getWidth();
        int height = bitmap.getHeight();

        ByteBuffer data = ByteBuffer.allocate(width * height * 4);
        Bitmap b = bitmap;

        if(bitmap.getConfig() != Bitmap.Config.ARGB_8888) {
            b = bitmap.copy(Bitmap.Config.ARGB_8888,true);
        }

        b.copyPixelsToBuffer(data);

        if(b != bitmap) {
            b.recycle();
        }

        setTexture(width,height,data.array(),1.0f);

    }

    public String url() {
        return url(_ptr);
    }

    private static final native void setStatus(long ptr,int status,String errmsg);
    private static final native String url(long ptr);

}
