package cn.kkmofang.game;

import android.util.Log;

import java.lang.ref.WeakReference;
import java.net.URI;

import cn.kkmofang.duktape.*;
import cn.kkmofang.duktape.Context;
import cn.kkmofang.script.IScriptFunction;
import cn.kkmofang.script.IScriptObject;
import cn.kkmofang.script.ScriptContext;
import cn.kkmofang.websocket.WebSocket;

/**
 * Created by hailong11 on 2018/4/16.
 */

public class JSWebSocket implements IScriptObject {

    private final static String[] _keys = new String[]{"connect","disconnect","write","on"};

    private final static IScriptFunction Alloc = new IScriptFunction() {

        @Override
        public int call() {

            Context ctx = (cn.kkmofang.duktape.Context) ScriptContext.currentContext();

            int top = ctx.getTop();

            if(top >0 && ctx.isString(-top)) {
                JSWebSocket v = new JSWebSocket(ctx.toString(-top));
                ctx.pushObject(v);
                return 1;
            }

            return 0;
        }
    };

    private final static IScriptFunction Connect = new IScriptFunction() {

        @Override
        public int call() {

            Context ctx = (cn.kkmofang.duktape.Context) ScriptContext.currentContext();

            ctx.pushThis();

            JSWebSocket v = (JSWebSocket) ctx.toObject(-1);

            ctx.pop();

            if(v != null) {
                v.getWebSocket().connect();
            }

            return 0;
        }
    };

    private final static IScriptFunction Disconnect = new IScriptFunction() {
        @Override
        public int call() {

            Context ctx = (cn.kkmofang.duktape.Context) ScriptContext.currentContext();

            ctx.pushThis();

            JSWebSocket v = (JSWebSocket) ctx.toObject(-1);

            ctx.pop();

            if(v != null) {
                v.getWebSocket().disconnect();
            }

            return 0;
        }
    };

    private final static IScriptFunction Write = new IScriptFunction() {
        @Override
        public int call() {

            Context ctx = (cn.kkmofang.duktape.Context) ScriptContext.currentContext();

            ctx.pushThis();

            JSWebSocket v = (JSWebSocket) ctx.toObject(-1);

            ctx.pop();

            if(v != null) {

                int top = ctx.getTop();

                if(top > 0) {

                    if(ctx.isString(-top)) {
                        v.getWebSocket().send(ctx.toString(-top));
                    } else if(ctx.isBytes(-top)) {
                        v.getWebSocket().send(ctx.toBytes(-top));
                    }

                }
            }

            return 0;
        }
    };

    private final static IScriptFunction On = new IScriptFunction() {
        @Override
        public int call() {

            Context ctx = (cn.kkmofang.duktape.Context) ScriptContext.currentContext();

            ctx.pushThis();

            JSWebSocket v = (JSWebSocket) ctx.toObject(-1);

            ctx.pop();

            if(v != null) {

                int top = ctx.getTop();

                String name = null;

                if(top > 0 && ctx.isString(-top) ) {
                    name = ctx.toString(-top);
                }

                Heapptr fn = null;

                if(top > 1 && ctx.isFunction(-top + 1)) {
                    fn = new Heapptr(ctx,ctx.getHeapptr(-top+1));
                }

                if(name != null) {

                    if("connected".equals(name)) {
                        v._onconnected = fn;
                    } else if("disconnected".equals(name)) {
                        v._ondisconnected = fn;
                    } else if("data".equals(name)) {
                        v._ondata = fn;
                    }

                }

            }

            return 0;
        }
    };

    private final WebSocket _webSocket;
    private Heapptr _onconnected;
    private Heapptr _ondisconnected;
    private Heapptr _ondata;

    public WebSocket getWebSocket() {
        return _webSocket;
    }

    public JSWebSocket(String url) {
        final WeakReference<JSWebSocket> v = new WeakReference<>(this);
        _webSocket = new WebSocket(URI.create(url), new WebSocket.Listener() {

            @Override
            public void onConnect() {

                JSWebSocket jsWebSocket = v.get();

                if(jsWebSocket != null && jsWebSocket._onconnected != null) {

                    Heapptr heapptr = jsWebSocket._onconnected;

                    cn.kkmofang.duktape.Context ctx = heapptr.context();

                    if(ctx != null) {
                        ScriptContext.pushContext(heapptr.context());

                        ctx.pushHeapptr(heapptr.heapptr());

                        if(ctx.isFunction(-1)) {

                            if(ctx.pcall(0) != Context.DUK_EXEC_SUCCESS) {
                                Log.d(cn.kkmofang.game.Context.TAG,ctx.getErrorString(-1));
                            }

                        }

                        ctx.pop();

                        ScriptContext.popContext();
                    }
                }
            }

            @Override
            public void onMessage(String message) {

                JSWebSocket jsWebSocket = v.get();

                if(jsWebSocket != null && jsWebSocket._ondata != null) {

                    Heapptr heapptr = jsWebSocket._ondata;

                    cn.kkmofang.duktape.Context ctx = heapptr.context();

                    if(ctx != null) {
                        ScriptContext.pushContext(heapptr.context());

                        ctx.pushHeapptr(heapptr.heapptr());

                        if(ctx.isFunction(-1)) {

                            ctx.push(message);

                            if(ctx.pcall(1) != Context.DUK_EXEC_SUCCESS) {
                                Log.d(cn.kkmofang.game.Context.TAG,ctx.getErrorString(-1));
                            }

                        }

                        ctx.pop();

                        ScriptContext.popContext();
                    }
                }

            }

            @Override
            public void onMessage(byte[] data) {

                JSWebSocket jsWebSocket = v.get();

                if(jsWebSocket != null && jsWebSocket._ondata != null) {

                    Heapptr heapptr = jsWebSocket._ondata;

                    cn.kkmofang.duktape.Context ctx = heapptr.context();

                    if(ctx != null) {
                        ScriptContext.pushContext(heapptr.context());

                        ctx.pushHeapptr(heapptr.heapptr());

                        if(ctx.isFunction(-1)) {

                            ctx.push(data);

                            if(ctx.pcall(1) != Context.DUK_EXEC_SUCCESS) {
                                Log.d(cn.kkmofang.game.Context.TAG,ctx.getErrorString(-1));
                            }

                        }

                        ctx.pop();

                        ScriptContext.popContext();
                    }
                }

            }

            @Override
            public void onDisconnect(int code, String reason) {

                JSWebSocket jsWebSocket = v.get();

                if(jsWebSocket != null && jsWebSocket._ondisconnected != null) {

                    Heapptr heapptr = jsWebSocket._ondisconnected;

                    cn.kkmofang.duktape.Context ctx = heapptr.context();

                    if(ctx != null) {
                        ScriptContext.pushContext(heapptr.context());

                        ctx.pushHeapptr(heapptr.heapptr());

                        if(ctx.isFunction(-1)) {

                            ctx.push(code);
                            ctx.push(reason);

                            if(ctx.pcall(2) != Context.DUK_EXEC_SUCCESS) {
                                Log.d(cn.kkmofang.game.Context.TAG,ctx.getErrorString(-1));
                            }

                        }

                        ctx.pop();

                        ScriptContext.popContext();
                    }
                }

            }

            @Override
            public void onError(Exception error) {

                JSWebSocket jsWebSocket = v.get();

                if(jsWebSocket != null && jsWebSocket._ondisconnected != null) {

                    Heapptr heapptr = jsWebSocket._ondisconnected;

                    cn.kkmofang.duktape.Context ctx = heapptr.context();

                    if(ctx != null) {
                        ScriptContext.pushContext(heapptr.context());

                        ctx.pushHeapptr(heapptr.heapptr());

                        if(ctx.isFunction(-1)) {

                            ctx.push(-1);
                            ctx.push(error.getMessage());

                            if(ctx.pcall(2) != Context.DUK_EXEC_SUCCESS) {
                                Log.d(cn.kkmofang.game.Context.TAG,ctx.getErrorString(-1));
                            }

                        }

                        ctx.pop();

                        ScriptContext.popContext();
                    }
                }

            }

        },null);
    }
    @Override
    public String[] keys() {
        return _keys;
    }

    @Override
    public Object get(String key) {
        if("connect".equals(key)) {
            return Connect;
        }
        if("disconnect".equals(key)) {
            return Disconnect;
        }
        if("write".equals(key)) {
            return Write;
        }
        if("on".equals(key)) {
            return On;
        }
        return null;
    }

    @Override
    public void set(String key, Object value) {

    }

    public final static void openlib(Context ctx) {

        ctx.pushGlobalObject();

        ctx.push("WebSocket");
        ctx.pushObject();

        ctx.push("alloc");
        ctx.pushFunction(Alloc);
        ctx.putProp(-3);

        ctx.putProp(-3);

        ctx.pop();
    }
}
