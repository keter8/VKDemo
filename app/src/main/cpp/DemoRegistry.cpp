#include "DemoRegistry.hpp"

DemoRegistry& DemoRegistry::instance() {
    static DemoRegistry inst;
    return inst;
}

void DemoRegistry::registerDemo(const std::string& id, DemoFactory f) {
    registry_[id] = std::move(f);
}

std::unique_ptr<Demo> DemoRegistry::create(const std::string& id) const {
    auto it = registry_.find(id);
    if (it == registry_.end()) return nullptr;
    return (it->second)();
}

std::vector<std::string> DemoRegistry::ids() const {
    std::vector<std::string> out;
    out.reserve(registry_.size());
    for (auto& kv : registry_) out.push_back(kv.first);
    return out;
}

const std::map<std::string, DemoFactory>& DemoRegistry::all() const {
    return registry_;
}

std::map<std::string, DemoFactory>& DemoRegistry::legacyMap() {
    return registry_;
}

// 相容層全域變數（reference）定義：讓舊寫法 demoRegistry["id"]=... 繼續可用
std::map<std::string, DemoFactory>& demoRegistry = DemoRegistry::instance().legacyMap();
