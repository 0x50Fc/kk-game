package cn.kkmofang.game;

/**
 * Created by hailong11 on 2018/6/25.
 */

public class Weak {

    protected final long _ptr;

    public Weak() {
        _ptr = alloc();
    }

    public void set(long object) {
        set(_ptr,object);
    }

    public long get() {
        return get(_ptr);
    }

    protected void finalize() throws Throwable {
        dealloc(_ptr);
        super.finalize();
    }

    private final static native long alloc();
    private final static native void dealloc(long ptr);
    private final static native void set(long ptr,long object);
    private final static native long get(long ptr);
}
