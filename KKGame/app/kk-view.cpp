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
#include "GAScene.h"
#include <glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include <libpng16/png.h>


namespace kk {
    
    static void view_error_callback(int error, const char* description) {
        kk::Log("[VIEW] %s",description);
    }
    
    static void view_GLFWframebuffersizefun(GLFWwindow * window,int inWidth,int inHeight) {
        
        kk::Application * app = (kk::Application *) glfwGetWindowUserPointer(window);
        
        glViewport(0,0,inWidth,inHeight);
        
        int wWidth,wHeight;
        
        glfwGetWindowSize(window, &wWidth, &wHeight);
        
        kk::GL::Float scale = inHeight / wHeight;
        kk::GL::Float width =  inWidth * 2.0f / scale;
        kk::GL::Float height =  inHeight * 2.0f / scale;
        kk::GL::Float dp = 1;
        
        kk::GL::Context * GLContext = app->GAContext();
        
        kk::GL::ContextState & s = GLContext->state();
        
        GLContext->setViewport(dp * width, dp * height);
        
        s.projection = glm::scale(kk::GL::mat4(1),kk::GL::vec3(2.0f / (dp *width),-2.0f / (dp * height),-0.0001));
        
    }
    
    static kk::GA::Point _focus = {0,0};
    
    static void view_GLFWscrollfun(GLFWwindow* window, double x,double y) {
        
        kk::Application * app = (kk::Application *) glfwGetWindowUserPointer(window);
        
        std::list<Strong> elements;
        
        app->document()->elementsByName("ga:scene", elements);
        
        if(!elements.empty()) {
            Strong & v = elements.front();
            kk::GA::Scene * scene = v.as<kk::GA::Scene>();
            if(scene) {
                _focus.x -= x * 10;
                _focus.y -= y * 10;
                if(_focus.x < scene->size.width * -0.5f) {
                    _focus.x =scene->size.width * -0.5f;
                }
                if(_focus.x > scene->size.width * 0.5f) {
                    _focus.x =scene->size.width * 0.5f;
                }
                if(_focus.y < scene->size.height * -0.5f) {
                    _focus.y =scene->size.height * -0.5f;
                }
                if(_focus.y > scene->size.height * 0.5f) {
                    _focus.y =scene->size.height * 0.5f;
                }
                scene->setPosition(_focus);
            }
        }
    }
    
    static kk::Int _cursorButton = -1;
    
  
    
    static void view_emitTouchEvent(GLFWwindow* window,kk::Application * app,kk::CString name) {
        
        kk::script::Context * jsContext = app->jsContext();
        
        kk::ElementEvent * event = new kk::ElementEvent();
        
        kk::Strong e = event;
        
        duk_context * ctx = jsContext->jsContext();
        
        duk_push_object(ctx);
        
        int width = 0;
        int height = 0;
        
        glfwGetWindowSize(window, &width, &height);
        
        duk_push_int(ctx, width);
        duk_put_prop_string(ctx, -2, "width");
        
        duk_push_int(ctx, height);
        duk_put_prop_string(ctx, -2, "height");
        
        duk_push_string(ctx, name);
        duk_put_prop_string(ctx, -2, "type");
        
        duk_push_int(ctx, 1);
        duk_put_prop_string(ctx, -2, "count");
        
        duk_push_array(ctx);
        
        duk_push_object(ctx);
        
        double x = 0;
        double y = 0;
        
        glfwGetCursorPos(window, &x, &y);
        
        duk_push_number(ctx, x);
        duk_put_prop_string(ctx, -2, "x");
        
        duk_push_number(ctx, y);
        duk_put_prop_string(ctx, -2, "y");
        
        duk_push_number(ctx, x * 2.0f / width -1.0f);
        duk_put_prop_string(ctx, -2, "dx");
        
        duk_push_number(ctx, y * 2.0f / height -1.0f);
        duk_put_prop_string(ctx, -2, "dy");
        
        duk_push_string(ctx,"1");
        duk_put_prop_string(ctx, -2, "id");
        
        duk_put_prop_index(ctx, -2, 0);
    
        duk_put_prop_string(ctx, -2, "items");
        
        event->data = new kk::script::Object(jsContext,-1);
        
        duk_pop(ctx);
        
        app->document()->emit(name, event);

    }
    
    static void view_GLFWcursorposfun(GLFWwindow* window,double x,double y) {
        
        kk::Application * app = (kk::Application *) glfwGetWindowUserPointer(window);
        
        if(_cursorButton != -1) {
            view_emitTouchEvent(window,app,"touchmove");
        }
        
    }
    
    static void view_GLFWmousebuttonfun(GLFWwindow* window,int button,int action,int mods) {
        
        kk::Application * app = (kk::Application *) glfwGetWindowUserPointer(window);
        
        if(button == 0) {
            if(action == GLFW_PRESS) {
                _cursorButton = button;
                view_emitTouchEvent(window,app,"touchstart");
            } else {
                if(_cursorButton == button) {
                    view_emitTouchEvent(window,app,"touchend");
                }
                _cursorButton = -1;
            }
        }
        
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
        
        glfwSetWindowUserPointer(window, app);
        
        glfwSetFramebufferSizeCallback(window, view_GLFWframebuffersizefun);
        glfwSetScrollCallback(window, view_GLFWscrollfun);
        glfwSetCursorPosCallback(window, view_GLFWcursorposfun);
        glfwSetMouseButtonCallback(window, view_GLFWmousebuttonfun);
        
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
        kk::GL::Float dp = 1;
        
        kk::GL::ContextState & s = GLContext->state();
        
        GLContext->setViewport(dp * width, dp * height);
        
        s.projection = glm::scale(kk::GL::mat4(1),kk::GL::vec3(2.0f / (dp *width),-2.0f / (dp * height),-0.0001));
        
        while (!glfwWindowShouldClose(window) && !app->isExiting()) {
            
            event_base_loop(base, EVLOOP_NONBLOCK);
            
            glClearColor(0, 0, 0, 0);
            
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
            if(app->isExiting()) {
                break;
            }
            
            app->exec();
            
            glFlush();
            
            glfwSwapBuffers(window);

            glfwPollEvents();
            
        }
        
        glfwDestroyWindow(window);
        
        glfwTerminate();
        
    }
    
}

#define PNG_BYTES_TO_CHECK 4

bool CreateTextureFromPng(const char* filename,GLuint textureID,kk::GL::Image * image) {
    
    FILE *fp;
    png_structp png_ptr;
    png_infop info_ptr;
    png_bytep* row_pointers;
    char buf[PNG_BYTES_TO_CHECK];
    int w, h, x, y, temp, color_type;
    
    fp = fopen( filename, "rb" );
    
    if( fp == NULL ) {
        return false;
    }
    
    png_ptr = png_create_read_struct( PNG_LIBPNG_VER_STRING, 0, 0, 0 );
    info_ptr = png_create_info_struct( png_ptr );
    
    setjmp( png_jmpbuf(png_ptr) );
    /* 读取PNG_BYTES_TO_CHECK个字节的数据 */
    temp = fread( buf, 1, PNG_BYTES_TO_CHECK, fp );
    /* 若读到的数据并没有PNG_BYTES_TO_CHECK个字节 */
    if( temp < PNG_BYTES_TO_CHECK ) {
        fclose(fp);
        png_destroy_read_struct( &png_ptr, &info_ptr, 0);
        return false;
    }
    /* 检测数据是否为PNG的签名 */
    temp = png_sig_cmp( (png_bytep)buf, (png_size_t)0, PNG_BYTES_TO_CHECK );
    /* 如果不是PNG的签名，则说明该文件不是PNG文件 */
    if( temp != 0 ) {
        fclose(fp);
        png_destroy_read_struct( &png_ptr, &info_ptr, 0);
        return false;
    }
    
    /* 复位文件指针 */
    rewind( fp );
    /* 开始读文件 */
    png_init_io( png_ptr, fp );
    /* 读取PNG图片信息和像素数据 */
    png_read_png( png_ptr, info_ptr, PNG_TRANSFORM_EXPAND, 0 );
    /* 获取图像的色彩类型 */
    color_type = png_get_color_type( png_ptr, info_ptr );
    /* 获取图像的宽高 */
    w = png_get_image_width( png_ptr, info_ptr );
    h = png_get_image_height( png_ptr, info_ptr );
    /* 获取图像的所有行像素数据，row_pointers里边就是rgba数据 */
    row_pointers = png_get_rows( png_ptr, info_ptr );
    
    unsigned char * rgba = (unsigned char *) malloc(w * h * 4);
    
    int pos = 0;
    
    /* 根据不同的色彩类型进行相应处理 */
    switch( color_type ) {
        case PNG_COLOR_TYPE_RGB_ALPHA:
            for( y=0; y<h; ++y ) {
                for( x=0; x<w*4; ) {
                    /* 以下是RGBA数据，需要自己补充代码，保存RGBA数据 */
                    rgba[pos++] = row_pointers[y][x++]; // red
                    rgba[pos++] = row_pointers[y][x++]; // green
                    rgba[pos++] = row_pointers[y][x++]; // blue
                    rgba[pos++] = row_pointers[y][x++]; // alpha
                }
            }
            break;
        case PNG_COLOR_TYPE_RGB:
            for( y=0; y<h; ++y ) {
                for( x=0; x<w*3; ) {
                    rgba[pos++] = row_pointers[y][x++]; // red
                    rgba[pos++] = row_pointers[y][x++]; // green
                    rgba[pos++] = row_pointers[y][x++]; // blue
                    rgba[pos++] = 0x0ff;
                }
            }
            break;
            /* 其它色彩类型的图像就不读了 */
        default:
            fclose(fp);
            png_destroy_read_struct( &png_ptr, &info_ptr, 0);
            return false;
    }
    
    png_destroy_read_struct( &png_ptr, &info_ptr, 0);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,w,h,0,GL_RGBA,GL_UNSIGNED_BYTE,rgba);

    image->setSize(w, h);
    
    free(rgba);
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


