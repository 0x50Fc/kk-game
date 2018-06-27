package cn.kkmofang.game;

import android.opengl.GLSurfaceView;

import java.lang.ref.WeakReference;

import cn.kkmofang.app.ILooper;

/**
 * Created by hailong11 on 2018/6/26.
 */

public class GLSurfaceViewLooper implements ILooper {

    private final WeakReference<GLSurfaceView> _view ;

    public GLSurfaceViewLooper(GLSurfaceView view) {
        _view = new WeakReference<>(view);
    }

    @Override
    public boolean post(Runnable runnable) {
        GLSurfaceView v = _view.get();
        if(v != null) {
            v.queueEvent(runnable);
            return true;
        }
        return false;
    }

    @Override
    public boolean postDelayed(final Runnable r, long delayMillis) {
        final GLSurfaceView v = _view.get();
        if(v != null) {
            return v.postDelayed(new Runnable() {
                @Override
                public void run() {
                    v.queueEvent(r);
                }
            },delayMillis);
        }
        return false;
    }

}
