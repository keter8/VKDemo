#include "DemoRegistry.hpp"
#include "HelloTriangle1.hpp"
#include "HelloTriangle2.hpp"

std::map<std::string, DemoFactory> demoRegistry = {
        {"triangle1", [](){ return std::make_unique<HelloTriangle1>(); }},
        {"triangle2", [](){ return std::make_unique<HelloTriangle2>(); }},
};
