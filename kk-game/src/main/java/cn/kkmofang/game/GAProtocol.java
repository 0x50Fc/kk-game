package cn.kkmofang.game;

import java.util.LinkedList;
import java.util.List;

/**
 * Created by hailong11 on 2018/6/25.
 */

public class GAProtocol {

    private static final List<OpenContext> _openContexts = new LinkedList<>();

    public static void openContext(Context context,GLViewElement element) {
        synchronized (_openContexts) {
            for(OpenContext v : _openContexts) {
                v.open(context,element);
            }
        }
    }

    public static void addOpenContext(OpenContext openContext) {
        synchronized (_openContexts) {
            _openContexts.add(openContext);
        }
    }

    public interface OpenContext {
        void open(Context context,GLViewElement element);
    }
}
