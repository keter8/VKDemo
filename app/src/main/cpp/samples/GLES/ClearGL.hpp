#pragma once
#include "demo.hpp"
#include <EGL/egl.h>
#include <GLES3/gl3.h> // 若要 GLES2 改成 <GLES2/gl2.h>

class ClearGL : public Demo {
public:
    const char* getName() const override { return "gl_clear"; }

    void init() override;
    void render() override;
    void destroy() override;

private:
    bool initEGLIfNeeded();
    void destroyEGL();

    EGLDisplay display_ = EGL_NO_DISPLAY;
    EGLSurface surface_ = EGL_NO_SURFACE;
    EGLContext context_ = EGL_NO_CONTEXT;
    int width_ = 0;
    int height_ = 0;
    float t_ = 0.f;
};
