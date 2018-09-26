package cn.kkmofang.game;

import android.graphics.Paint;
import android.graphics.Typeface;

import cn.kkmofang.view.value.Pixel;

/**
 * Created by hailong11 on 2018/7/19.
 */

public class TextPaint extends android.text.TextPaint {

    public float maxWidth = 0;
    public int strokeColor = 0;
    public float strokeWidth = 0;
    public int color = 0;

    public TextPaint() {
        super(Paint.ANTI_ALIAS_FLAG);
    }

    public void set(String family,float fontSize,boolean italic,boolean bold, int color,float maxWidth,int strokeColor,float strokeWidth) {

        if(family != null && !"".equals(family)) {
            if(italic && bold) {
                setTypeface(Typeface.create(family,Typeface.BOLD_ITALIC));
            } else if(bold){
                setTypeface(Typeface.create(family,Typeface.BOLD));
            } else if(italic){
                setTypeface(Typeface.create(family,Typeface.ITALIC));
            } else {
                setTypeface(Typeface.create(family,Typeface.NORMAL));
            }
        } else if(italic && bold) {
            setTypeface(Typeface.create(Typeface.DEFAULT,Typeface.BOLD_ITALIC));
        } else if(bold){
            setTypeface(Typeface.create(Typeface.DEFAULT,Typeface.BOLD));
        } else if(italic){
            setTypeface(Typeface.create(Typeface.DEFAULT,Typeface.ITALIC));
        } else {
            setTypeface(Typeface.create(Typeface.DEFAULT,Typeface.NORMAL));
        }

        setFakeBoldText(bold);
        setColor(color & 0x00ffffff);
        setAlpha((color >> 24) & 0x0ff);
        setTextSize(fontSize);
        this.color = color;
        this.maxWidth = maxWidth;
        this.strokeColor = strokeColor;
        this.strokeWidth = strokeWidth;
    }
}
