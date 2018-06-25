package cn.kkmofang.game;

/**
 * Created by hailong11 on 2018/6/25.
 */

public class WeakTexture extends Weak {

    public int getId() {
        return getId(_ptr);
    }

    private final static native int getId(long ptr);
}
