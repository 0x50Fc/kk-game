package cn.kkmofang.game;

/**
 * Created by hailong11 on 2018/6/25.
 */

public class WeakTexture extends Weak {

    public void setTexture(int width,int height,byte[] data) {
        setTexture(_ptr,width,height,data);
    }


    private final static native void setTexture(long ptr,int width,int height,byte[] data);
}
