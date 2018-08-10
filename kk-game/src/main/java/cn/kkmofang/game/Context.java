package cn.kkmofang.game;

import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.RectF;
import android.graphics.drawable.BitmapDrawable;
import android.graphics.drawable.Drawable;
import android.text.Layout;
import android.text.StaticLayout;

import java.lang.ref.WeakReference;
import java.nio.ByteBuffer;

import cn.kkmofang.app.AsyncCaller;
import cn.kkmofang.app.ILooper;
import cn.kkmofang.app.IRecycle;
import cn.kkmofang.app.JSWebSocket;
import cn.kkmofang.image.Image;
import cn.kkmofang.image.ImageStyle;
import cn.kkmofang.script.IScriptFunction;
import cn.kkmofang.script.ScriptContext;
import cn.kkmofang.view.IViewContext;
import cn.kkmofang.view.ImageCallback;

/**
 * Created by hailong11 on 2018/4/10.
 */

public class Context extends cn.kkmofang.duktape.BasicContext implements IRecycle {

    public final static String TAG = "kk";

    public final IViewContext viewContext;
    public final String basePath;

    private final ILooper _looper;

    static {
        System.loadLibrary("duktape");
        System.loadLibrary("event");
        System.loadLibrary("kk-object");
        System.loadLibrary("kk-element");
        System.loadLibrary("kk-game");
    }

    public Context(ILooper looper, IViewContext viewContext,String basePath) {
        super(alloc(viewContext.getAbsolutePath(basePath)));
        _looper = looper;
        this.viewContext = viewContext;
        this.basePath = basePath.endsWith("/") ? basePath : basePath + "/";
    }

    public void post(final Runnable run) {
        final WeakReference<Context> v= new WeakReference<>(this);
        _looper.post(new Runnable() {
            @Override
            public void run() {
                Context ctx = v.get();
                if (ctx != null) {
                    ScriptContext.pushContext(ctx);
                    run.run();
                    ScriptContext.popContext();
                }
            }
        });

    }

    public void exec() {
        ScriptContext.pushContext(this);
        exec(_ptr);
        ScriptContext.popContext();
    }

    public void run() {
        ScriptContext.pushContext(this);
        run(_ptr);
        ScriptContext.popContext();
    }

    public void setViewport(float width,float height,float scale) {
        setViewport(_ptr,width,height,scale);
    }

    public void recycle() {
        dealloc(_ptr);
    }

    protected void GLImageGen(WeakImage image,Drawable drawable) {

        Bitmap v = null;

        if(drawable instanceof BitmapDrawable) {
            v =  ((BitmapDrawable)(drawable)).getBitmap();
        } if(drawable instanceof Image) {
            v =  ((Image)(drawable)).getBitmap();
        }

        if(v != null && !v.isRecycled()) {

            image.setBitmap(v);
            image.setStatus(WeakImage.STATUS_LOADED,"");

        } else {
            image.setStatus(WeakImage.STATUS_FAIL,"不支持的图片格式");
        }

    }

    public static void ContextGetImage(long object) {

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

    public void emit(final String name,final Object data) {
        final WeakReference<Context> v= new WeakReference<>(this);
        post(new Runnable() {
            @Override
            public void run() {
                Context ctx = v.get();
                if(ctx != null) {
                    ctx.push(name);
                    ctx.pushValue(data);
                    ctx.emit(ctx.ptr());
                }
            }
        });
    }

    public void on(final String name,final IScriptFunction fn) {
        final WeakReference<Context> v= new WeakReference<>(this);
        post(new Runnable() {
            @Override
            public void run() {
                Context ctx = v.get();
                if(ctx != null) {
                    ctx.push(name);
                    ctx.pushObject(fn);
                    ctx.on(ctx.ptr());
                }
            }
        });
    }

    public void off(final String name) {
        final WeakReference<Context> v= new WeakReference<>(this);
        post(new Runnable() {
            @Override
            public void run() {
                Context ctx = v.get();
                if(ctx != null) {
                    ctx.push(name);
                    ctx.off(ctx.ptr());
                }
            }
        });
    }

    public float loadingProgress() {
        return loadingProgress(_ptr);
    }

    public final static void ContextGetStringTexture(long textureId,String text,TextPaint paint) {

        if(text == null) {
            text = "";
        }

        float maxWidth = paint.maxWidth;

        if(maxWidth == 0) {
            maxWidth = Integer.MAX_VALUE;
        }

        WeakTexture texture = new WeakTexture();
        texture.set(textureId);

        StaticLayout layout = new StaticLayout(
                text,
                0,
                text.length(),
                paint,
                (int) Math.ceil(maxWidth),
                Layout.Alignment.ALIGN_NORMAL,
                1.0f,
                0f,
                false);

        float width = 0;

        for(int i=0;i<layout.getLineCount();i++) {
            float v = layout.getLineWidth(i);
            if(v > width) {
                width = v;
            }
        }

        int mWidth = (int) Math.ceil(width);
        int mHeight = layout.getHeight();

        Bitmap bitmap = Bitmap.createBitmap(mWidth,mHeight, Bitmap.Config.ARGB_8888);

        Canvas canvas = new Canvas(bitmap);

        if(paint.strokeWidth > 0) {

            paint.setStyle(Paint.Style.FILL_AND_STROKE);
            paint.setStrokeWidth(paint.strokeWidth);
            paint.setColor(paint.strokeColor & 0x0ffffff);
            paint.setAlpha((paint.strokeColor >> 24) & 0x0ff);
            layout.draw(canvas);

            paint.setStrokeWidth(0);
            paint.setColor(paint.color & 0x0ffffff);
            paint.setAlpha((paint.color >> 24) & 0x0ff);

        }

        layout.draw(canvas);

        ByteBuffer data = ByteBuffer.allocate(mWidth * mHeight * 4);

        bitmap.copyPixelsToBuffer(data);

        bitmap.recycle();

        texture.setTexture(mWidth,mHeight,data.array());

    }

    private final static native long alloc(String basePath);
    private final static native void dealloc(long ptr);
    private final static native void exec(long ptr);
    private final static native void setViewport(long ptr, float width,float height,float scale);
    private final static native void run(long ptr);
    private final static native void emit(long ptr);
    private final static native void on(long ptr);
    private final static native void off(long ptr);
    private final static native float loadingProgress(long ptr);
}
