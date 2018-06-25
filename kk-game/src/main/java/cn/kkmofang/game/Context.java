package cn.kkmofang.game;

import android.graphics.Bitmap;
import android.graphics.drawable.BitmapDrawable;
import android.graphics.drawable.Drawable;
import android.opengl.GLUtils;
import android.os.Handler;

import java.lang.ref.WeakReference;

import javax.microedition.khronos.opengles.GL10;

import cn.kkmofang.app.IRecycle;
import cn.kkmofang.app.JSWebSocket;
import cn.kkmofang.image.Image;
import cn.kkmofang.image.ImageStyle;
import cn.kkmofang.script.ScriptContext;
import cn.kkmofang.view.IViewContext;
import cn.kkmofang.view.ImageCallback;

/**
 * Created by hailong11 on 2018/4/10.
 */

public class Context extends cn.kkmofang.duktape.Context implements IRecycle {

    public final static String TAG = "kk";

    public final IViewContext viewContext;
    public final String basePath;

    public final JSWebSocket jsWebSocket;
    private final GL10 _gl10;
    private final Handler _handler;

    static {
        System.loadLibrary("kk-game");
    }

    public Context(IViewContext viewContext,String basePath,GL10 gl10) {
        super(alloc(basePath));
        this.viewContext = viewContext;
        this.basePath = basePath.endsWith("/") ? basePath : basePath + "/";
        _gl10 = gl10;
        _handler =  new Handler();
        jsWebSocket = new JSWebSocket();

        {
            pushGlobalObject();
            push("WebSocket");
            pushObject(jsWebSocket);
            putProp(-3);
            pop();
        }
    }

    @Override
    protected void finalize() throws Throwable {
        dealloc(_ptr);
        super.finalize();
    }

    public void post(final Runnable run) {
        final  WeakReference<Context> v= new WeakReference<Context>(this);

        _handler.post(new Runnable() {
            @Override
            public void run() {
                Context ctx = v.get();
                if(ctx != null) {
                    ScriptContext.pushContext(ctx);
                    run.run();
                    ScriptContext.popContext();
                }
            }
        });
    }

    public void exec() {
        exec(_ptr);
    }

    public void run() {
        run(_ptr);
    }

    public void setViewport(float width,float height) {
        setViewport(_ptr,width,height);
    }

    public void recycle() {
        jsWebSocket.recycle();
    }

    protected void GLImageGen(WeakImage image,Drawable drawable) {

        Bitmap v = null;

        if(drawable instanceof BitmapDrawable) {
            v =  ((BitmapDrawable)(drawable)).getBitmap();
        } if(drawable instanceof Image) {
            v =  ((Image)(drawable)).getBitmap();
        }


        if(v != null && !v.isRecycled()) {

            int id = image.getId();

            if(id !=0 ){
                _gl10.glBindTexture(GL10.GL_TEXTURE_2D,id);
                GLUtils.texImage2D(GL10.GL_TEXTURE_2D,0,v,0);
            }

            image.setSize(v.getWidth(),v.getHeight());
            image.setStatus(WeakImage.STATUS_LOADED,"");

        } else {
            image.setStatus(WeakImage.STATUS_FAIL,"不支持的图片格式");
        }

    }

    public final static void ContextGetImage(long object) {

        Context context = (Context) ScriptContext.currentContext();

        final WeakImage image = new WeakImage();

        final WeakReference<Context> ctx = new WeakReference<Context>(context);

        image.set(object);

        String uri = image.url();

        if(uri == null) {
            image.setStatus(WeakImage.STATUS_FAIL,"未找到图片");
            return;
        } else if(uri.startsWith("http://") || uri.startsWith("https://")) {

        } else {
            uri = context.basePath + uri;
        }

        context.viewContext.getImage(uri, new ImageStyle(context.viewContext.getContext()), new ImageCallback() {

            @Override
            public void onImage(final Drawable drawable) {

                Context v = ctx.get();

                if(v != null) {
                    v.post(new Runnable() {
                        @Override
                        public void run() {
                            Context v = ctx.get();

                            if(v != null) {
                                v.GLImageGen(image, drawable);
                            }
                        }
                    });
                }

            }

            @Override
            public void onException(Exception exception) {
                image.setStatus(WeakImage.STATUS_FAIL,exception.getLocalizedMessage());
            }
        });

    }

    public final static void ContextGetStringTexture(long texture) {

    }

    private final static native long alloc(String basePath);
    private final static native void dealloc(long ptr);
    private final static native void exec(long ptr);
    private final static native void setViewport(long ptr, float width,float height);
    private final static native void run(long ptr);


}
