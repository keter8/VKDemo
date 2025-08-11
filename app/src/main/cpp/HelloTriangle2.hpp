#pragma once
#include "Demo.hpp"

class HelloTriangle2 : public Demo {
public:
    const char* getName() const override { return "triangle2"; }
    void init() override;
    void render() override;
    void destroy() override;
};

void HelloTriangle2::init() { /* Vulkan 資源建立 */ }
void HelloTriangle2::render() { /* 畫三角形1 */ }
void HelloTriangle2::destroy() { /* 釋放資源 */ }
