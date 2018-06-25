package cn.kkmofang.game;

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

    public void setSize(int width,int height) {
        setSize(_ptr,width,height);
    }

    public String url() {
        return url(_ptr);
    }

    private static final native void setStatus(long ptr,int status,String errmsg);
    private static final native void setSize(long ptr,int width,int height);
    private static final native String url(long ptr);

}
