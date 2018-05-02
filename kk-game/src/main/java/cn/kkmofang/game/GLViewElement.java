package cn.kkmofang.game;

import android.opengl.GLSurfaceView;
import cn.kkmofang.view.ViewElement;

/**
 * Created by hailong11 on 2018/4/17.
 */

public class GLViewElement extends ViewElement {

    public Class<?> viewClass() {
        return GLSurfaceView.class;
    }
}
