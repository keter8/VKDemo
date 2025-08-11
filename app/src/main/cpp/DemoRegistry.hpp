#pragma once
#include <map>
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include "demo.hpp"

using DemoFactory = std::function<std::unique_ptr<Demo>()>;

class DemoRegistry {
public:
    static DemoRegistry& instance();

    // 新 API
    void registerDemo(const std::string& id, DemoFactory f);
    std::unique_ptr<Demo> create(const std::string& id) const;
    std::vector<std::string> ids() const;
    const std::map<std::string, DemoFactory>& all() const;

    // 相容層：提供 map 參考，給舊程式碼沿用
    std::map<std::string, DemoFactory>& legacyMap();

private:
    DemoRegistry() = default;
    DemoRegistry(const DemoRegistry&) = delete;
    DemoRegistry& operator=(const DemoRegistry&) = delete;

    std::map<std::string, DemoFactory> registry_;
};

// 相容層：舊程式碼用到的全域變數（reference），綁到內部 map
extern std::map<std::string, DemoFactory>& demoRegistry;
