#include "samples/GLES/ClearGL.hpp"
#include <android/log.h>
#include <cmath>

#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "ClearGL", __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,  "ClearGL", __VA_ARGS__)

// 從 main.cpp 取得目前的 ANativeWindow*
extern ANativeWindow* GetCurrentNativeWindow();

static bool choose_config(EGLDisplay dpy, EGLConfig* out_cfg) {
    const EGLint attribs[] = {
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
            EGL_SURFACE_TYPE,    EGL_WINDOW_BIT,
            EGL_RED_SIZE,8, EGL_GREEN_SIZE,8, EGL_BLUE_SIZE,8, EGL_ALPHA_SIZE,8,
            EGL_DEPTH_SIZE,24, EGL_STENCIL_SIZE,8,
            EGL_NONE
    };
    EGLint n=0;
    if (eglChooseConfig(dpy, attribs, out_cfg, 1, &n) && n>0) return true;

    const EGLint attribs2[] = {
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL_SURFACE_TYPE,    EGL_WINDOW_BIT,
            EGL_RED_SIZE,8, EGL_GREEN_SIZE,8, EGL_BLUE_SIZE,8, EGL_ALPHA_SIZE,8,
            EGL_DEPTH_SIZE,24, EGL_STENCIL_SIZE,8,
            EGL_NONE
    };
    return eglChooseConfig(dpy, attribs2, out_cfg, 1, &n) && n>0;
}

bool ClearGL::initEGLIfNeeded() {
    ANativeWindow* window = GetCurrentNativeWindow();
    if (!window) {
        LOGE("No ANativeWindow available");
        return false;
    }

    display_ = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (display_ == EGL_NO_DISPLAY) return false;
    if (!eglInitialize(display_, nullptr, nullptr)) return false;

    EGLConfig cfg{};
    if (!choose_config(display_, &cfg)) return false;

    const EGLint ctx_es3[] = { EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE };
    context_ = eglCreateContext(display_, cfg, EGL_NO_CONTEXT, ctx_es3);
    if (context_ == EGL_NO_CONTEXT) {
        const EGLint ctx_es2[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
        context_ = eglCreateContext(display_, cfg, EGL_NO_CONTEXT, ctx_es2);
        if (context_ == EGL_NO_CONTEXT) return false;
    }

    surface_ = eglCreateWindowSurface(display_, cfg, window, nullptr);
    if (surface_ == EGL_NO_SURFACE) return false;

    if (!eglMakeCurrent(display_, surface_, surface_, context_)) return false;

    eglQuerySurface(display_, surface_, EGL_WIDTH,  &width_);
    eglQuerySurface(display_, surface_, EGL_HEIGHT, &height_);
    glViewport(0, 0, width_, height_);
    return true;
}

void ClearGL::destroyEGL() {
    if (display_ != EGL_NO_DISPLAY) {
        eglMakeCurrent(display_, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    }
    if (surface_ != EGL_NO_SURFACE) eglDestroySurface(display_, surface_);
    if (context_ != EGL_NO_CONTEXT) eglDestroyContext(display_, context_);
    if (display_ != EGL_NO_DISPLAY) eglTerminate(display_);

    surface_ = EGL_NO_SURFACE;
    context_ = EGL_NO_CONTEXT;
    display_ = EGL_NO_DISPLAY;
}

void ClearGL::init() {
    initEGLIfNeeded();
}

void ClearGL::render() {
    if (display_ == EGL_NO_DISPLAY || surface_ == EGL_NO_SURFACE) return;

    t_ += 0.016f; // 假設約 60fps
    float r = 0.2f + 0.2f * sinf(t_);
    float g = 0.3f + 0.3f * sinf(t_ * 0.7f);
    float b = 0.4f + 0.4f * sinf(t_ * 1.3f);

    glClearColor(r, g, b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    eglSwapBuffers(display_, surface_);
}

void ClearGL::destroy() {
    destroyEGL();
}
