package cn.kkmofang.game;

/**
 * Created by hailong11 on 2018/4/10.
 */

public class Context extends cn.kkmofang.duktape.Context {

    public final static String TAG = "kk";

    static {
        System.loadLibrary("kk-game");
    }

    public Context() {
        super(alloc());
    }

    @Override
    protected void finalize() throws Throwable {
        dealloc(_ptr);
        super.finalize();
    }

    private final static native long alloc();
    private final static native void dealloc(long ptr);
    private final static native void exec(long ptr);
    private final static native void setViewport(long ptr, float width,float height);

}
