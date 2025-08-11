#pragma once

#include <map>
#include <string>
#include <memory>
#include <functional>
#include "demo.hpp"

// DemoFactory 跟 demoRegistry 必須有 extern 宣告
using DemoFactory = std::function<std::unique_ptr<Demo>()>;

extern std::map<std::string, DemoFactory> demoRegistry;
