#pragma once
#include "Demo.hpp"

class HelloTriangle1 : public Demo {
public:
    const char* getName() const override { return "triangle1"; }
    void init() override;
    void render() override;
    void destroy() override;
};

void HelloTriangle1::init() { /* Vulkan 資源建立 */ }
void HelloTriangle1::render() { /* 畫三角形1 */ }
void HelloTriangle1::destroy() { /* 釋放資源 */ }
