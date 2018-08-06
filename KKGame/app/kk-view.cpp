//
//  view.cpp
//  app
//
//  Created by 张海龙 on 2018/8/5.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#include "kk-config.h"
#include "kk-app.h"
#include "kk-ev.h"
#include "kk-view.h"
#include "kk-string.h"
#include <glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include <libpng16/png.h>

namespace kk {
    
    static void view_error_callback(int error, const char* description) {
        kk::Log("[VIEW] %s",description);
    }
    
    void view(kk::Application * app, struct event_base * base, struct evdns_base * dns) {
        
        GLFWwindow* window;
        
        int inWidth, inHeight;

        glfwSetErrorCallback(view_error_callback);
        
        if (!glfwInit())
            exit(EXIT_FAILURE);
        
        window = glfwCreateWindow(812, 375, "kk-app", NULL, NULL);
        
        if (!window) {
            glfwTerminate();
            return;
        }
        
        glfwMakeContextCurrent(window);

        glfwSwapInterval(1);
        
        glfwGetFramebufferSize(window, &inWidth, &inHeight);
        
        glViewport(0,0,inWidth,inHeight);
        
        glDisable(GL_DEPTH_TEST);
        
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE , GL_ONE_MINUS_SRC_ALPHA);
        
        kk::GL::Context * GLContext = app->GAContext();
        
        GLContext->init();
        
        kk::GL::Float scale = inHeight / 375.0;
        kk::GL::Float width =  inWidth * 2.0f / scale;
        kk::GL::Float height =  inHeight * 2.0f / scale;
        kk::GL::Float dp = 750.0f / MIN(width,height) ;
        
        kk::GL::ContextState & s = GLContext->state();
        
        GLContext->setViewport(dp * width, dp * height);
        
        s.projection = glm::scale(kk::GL::mat4(1),kk::GL::vec3(2.0f / (dp *width),2.0f / (dp * height),-0.0001));
        
        while (!glfwWindowShouldClose(window)) {
            
            event_base_loop(base, EVLOOP_NONBLOCK);
            
            glClearColor(0, 0, 0, 0);
            
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
            app->exec();
            
            glFlush();
            
            glfwSwapBuffers(window);

            glfwPollEvents();
            
        }
        
        glfwDestroyWindow(window);
        
        glfwTerminate();
        
    }
    
}

bool CreateTextureFromPng(const char* filename,GLuint textureID,kk::GL::Image * image) {
    unsigned char header[8];     //8
    int k;   //用于循环
    int width, height; //记录图片到宽和高
    png_byte color_type; //图片到类型（可能会用在是否是开启来通道）
    png_byte bit_depth; //字节深度
    
    png_structp png_ptr; //图片
    png_infop info_ptr; //图片的信息
    int number_of_passes; //隔行扫描
    png_bytep * row_pointers;//图片的数据内容
    int row,col,pos;  //用于改变png像素排列的问题。
    GLubyte *rgba;
    
    FILE *fp=fopen(filename,"rb");//以只读形式打开文件名为file_name的文件
    if(!fp)//做出相应可能的错误处理
    {
        fclose(fp);//关闭打开的文件！给出默认贴图
        return false;//此处应该调用一个生成默认贴图返回ID的函数
    }
    //读取文件头判断是否所png图片.不是则做出相应处理
    fread(header, 1, 8, fp);
    if(png_sig_cmp(header,0,8))
    {
        fclose(fp);
        return false; //每个错误处理都是一样的！这样报错之后锁定就要花点小时间来！
    }
    
    //根据libpng的libpng-manual.txt的说明使用文档 接下来必须初始化png_structp 和 png_infop
    png_ptr=png_create_read_struct(PNG_LIBPNG_VER_STRING,NULL,NULL,NULL); //后三个是绑定错误以及警告的函数这里设置为空
    if(!png_ptr)//做出相应到初始化失败的处理
    {
        fclose(fp);
        return false;
    }
    //根据初始化的png_ptr初始化png_infop
    info_ptr=png_create_info_struct(png_ptr);
    
    if(!info_ptr)
    {
        //初始化失败以后销毁png_structp
        png_destroy_read_struct(&png_ptr,(png_infopp)NULL,(png_infopp)NULL);
        fclose(fp);
        return false;
    }
    
    
    //老老实实按照libpng给到的说明稳定步骤来  错误处理！
    if (setjmp(png_jmpbuf(png_ptr)))
        
    {
        //释放占用的内存！然后关闭文件返回一个贴图ID此处应该调用一个生成默认贴图返回ID的函数
        
        png_destroy_read_struct(&png_ptr,(png_infopp)NULL,(png_infopp)NULL);
        
        fclose(fp);
        
        return false;
        
    }
    //你需要确保是通过2进制打开的文件。通过i/o定制函数png_init_io
    png_init_io(png_ptr,fp);
    //似乎是说要告诉libpng文件从第几个开始missing
    png_set_sig_bytes(png_ptr, 8);
    //如果你只想简单的操作你现在可以实际读取图片信息了！
    png_read_info(png_ptr, info_ptr);
    //获得图片到信息 width height 颜色类型  字节深度
    width = png_get_image_width(png_ptr, info_ptr);
    height = png_get_image_height(png_ptr, info_ptr);
    color_type = png_get_color_type(png_ptr, info_ptr);
    //如果图片带有alpha通道就需要
    // if (color_type == PNG_COLOR_TYPE_RGB_ALPHA)
    
    // png_set_swap_alpha(png_ptr);
    bit_depth = png_get_bit_depth(png_ptr, info_ptr);
    //隔行扫描图片  这个必须要调用才能进行
    number_of_passes = png_set_interlace_handling(png_ptr);
    //将读取到的信息更新到info_ptr
    png_read_update_info(png_ptr, info_ptr);
    
    //读文件
    if (setjmp(png_jmpbuf(png_ptr))){
        fclose(fp);
        return false;
    }
    rgba=(GLubyte*)malloc(width * height * 4);
    //使用动态数组  设置长度
    row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * height);
    
    for (k = 0; k < height; k++)
        row_pointers[k] = NULL;
    
    //通过扫描流里面的每一行将得到的数据赋值给动态数组
    for (k=0; k<height; k++)
        //row_pointers[k] = (png_byte*) malloc(png_get_rowbytes(png_ptr,info_ptr));
        row_pointers[k] = (png_bytep)png_malloc(png_ptr, png_get_rowbytes(png_ptr,
                                                                          info_ptr));
    //由于png他的像素是由 左-右-从顶到底 而贴图需要的像素都是从左-右-底到顶的所以在这里需要把像素内容进行一个从新排列
    //读图片
    png_read_image(png_ptr, row_pointers);
    
    pos = (width * height * 4) - (4 * width);
    for( row = 0; row < height; row++)
    {
        for( col = 0; col < (4 * width); col += 4)
        {
            rgba[pos++] = row_pointers[row][col];        // red
            rgba[pos++] = row_pointers[row][col + 1];    // green
            rgba[pos++] = row_pointers[row][col + 2];    // blue
            rgba[pos++] = row_pointers[row][col + 3];    // alpha
        }
        pos=(pos - (width * 4)*2);
    }
    
    
    //绑定纹理
    glBindTexture(GL_TEXTURE_2D,textureID); //将纹理绑定到名字
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    //设置纹理所用到图片数据
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,width,height,0,GL_RGBA,GL_UNSIGNED_BYTE,rgba);

    image->setSize(width, height);
    
    free(row_pointers);
    fclose(fp);
    
    return true;
}

void kk::GL::ContextGetImage(kk::GL::Context * context, kk::GL::Image * image) {
    
    kk::String path = context->absolutePath(image->uri());
    
    if( kk::CStringHasSuffix(path.c_str(), ".png") ) {
        if(CreateTextureFromPng(path.c_str(),image->texture(),image)) {
            image->setStatus(kk::GL::ImageStatusLoaded);
        } else {
            image->setStatus(kk::GL::ImageStatusFail);
        }
    } else {
        assert(0);
    }
    
}

void kk::GL::ContextGetStringTexture(kk::GL::Context * context,kk::GL::Texture * texture ,kk::CString text, kk::GL::Paint & paint) {
    
}

