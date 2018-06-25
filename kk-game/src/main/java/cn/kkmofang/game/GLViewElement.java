package cn.kkmofang.game;

import android.app.Activity;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.view.View;

import java.lang.ref.WeakReference;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import cn.kkmofang.view.Element;
import cn.kkmofang.view.ViewElement;

/**
 * Created by hailong11 on 2018/4/17.
 */

public class GLViewElement extends ViewElement {

    private Context _context;
    private String _basePath;

    public Class<?> viewClass() {
        return GLSurfaceView.class;
    }

    public void onPause(Activity activity) {
        View v = view();
        if(v != null && v instanceof GLSurfaceView) {
            ((GLSurfaceView) v).onPause();
        }
        super.onPause(activity);
    }

    public void onResume(Activity activity) {
        View v = view();
        if(v != null && v instanceof GLSurfaceView) {
            ((GLSurfaceView) v).onResume();
        }
        super.onResume(activity);
    }

    public void setView(View view) {

        View v = view();

        if(v != null && v instanceof GLSurfaceView) {
            ((GLSurfaceView) v).setRenderer(null);
        }

        _context = null;

        super.setView(view);

        if(view != null && view instanceof GLSurfaceView) {

            _basePath = get("path");

            final WeakReference<GLViewElement> e = new WeakReference<>(this);

            GLSurfaceView glView = (GLSurfaceView) view;

            glView.setEGLContextClientVersion(2);

            glView.setRenderer(new GLSurfaceView.Renderer() {

                @Override
                public void onSurfaceCreated(GL10 gl10, EGLConfig eglConfig) {
                    GLViewElement v = e.get();
                    if(v != null) {
                        v.onSurfaceCreated(gl10,eglConfig);
                    }
                }

                @Override
                public void onSurfaceChanged(GL10 gl10, int width, int height) {
                    GLViewElement v = e.get();
                    if(v != null) {
                        v.onSurfaceChanged(gl10,width,height);
                    }
                }

                @Override
                public void onDrawFrame(GL10 gl10) {
                    GLViewElement v = e.get();
                    if(v != null) {
                        v.onDrawFrame(gl10);
                    }
                }

            });

        }
    }

    protected void onSurfaceCreated(GL10 gl10, EGLConfig eglConfig) {
        _context = new Context(viewContext,_basePath,gl10);
        GAProtocol.openContext(_context,this);
        _context.run();
    }

    protected void onSurfaceChanged(GL10 gl10, int width, int height) {
        _context.setViewport(width,height);
    }

    protected void onDrawFrame(GL10 gl10) {

        gl10.glClearColor(0,0,0,0);
        gl10.glClear(GL10.GL_COLOR_BUFFER_BIT | GL10.GL_DEPTH_BUFFER_BIT);

        _context.exec();

    }
}
