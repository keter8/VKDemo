#include "DemoRegistry.hpp"
#include "samples/GLES/ClearGL.hpp"

static void register_gles_samples() {
    DemoRegistry::instance().registerDemo("gles/clear", [] {
        return std::make_unique<ClearGL>();
    });
}
struct AutoReg_GLES { AutoReg_GLES(){ register_gles_samples(); } };
static AutoReg_GLES s_autoreg;
