#include <jni.h>
#include <memory>
#include <string>
#include <map>
#include <functional>
#include <android/native_window.h>
#include <android/log.h>

#include "DemoRegistry.hpp"
#include "demo.hpp"

#include <game-activity/GameActivity.cpp>
#include <game-text-input/gametextinput.cpp>

#include <game-activity/native_app_glue/android_native_app_glue.c>

#define LOG_TAG "VKDemo"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

// ---------------- 全域狀態 ----------------
static ANativeWindow* g_appWindow = nullptr;     // 當前的視窗
static std::unique_ptr<Demo> currentDemo;        // 目前執行中的 Demo
static std::string g_pendingDemoId;              // 視窗未就緒時要啟動的 demo_id

extern std::map<std::string, DemoFactory>& demoRegistry;

// 讓其他地方（像 ClearGL）可取得視窗
ANativeWindow* GetCurrentNativeWindow() {
    return g_appWindow;
}

// ---------------- Demo 控制 ----------------
void selectDemo(const std::string& demo_id) {
    LOGI("selectDemo(%s)", demo_id.c_str());

    if (!g_appWindow) {
        g_pendingDemoId = demo_id;
        LOGI("Window not ready. Deferring demo init.");
        return;
    }

    auto it = demoRegistry.find(demo_id);
    if (it != demoRegistry.end()) {
        if (currentDemo) currentDemo->destroy();
        currentDemo = it->second();
        currentDemo->init();
        LOGI("Started demo: %s", currentDemo->getName());
    } else {
        LOGE("Demo not found: %s", demo_id.c_str());
    }
}

// ---------------- 從 Intent 取得 demo_id ----------------
void get_demo_id_from_intent(android_app* pApp, std::string& demo_id_out) {
    JNIEnv* env = nullptr;
    pApp->activity->vm->AttachCurrentThread(&env, nullptr);

    GameActivity* ga = pApp->activity;
    jobject javaActivity = ga->javaGameActivity;

    jclass activityClass = env->GetObjectClass(javaActivity);
    jmethodID getIntent = env->GetMethodID(activityClass, "getIntent", "()Landroid/content/Intent;");
    jobject intent = env->CallObjectMethod(javaActivity, getIntent);

    jclass intentClass = env->GetObjectClass(intent);
    jmethodID getStringExtra = env->GetMethodID(intentClass, "getStringExtra",
                                                "(Ljava/lang/String;)Ljava/lang/String;");
    jstring jDemoId = (jstring)env->CallObjectMethod(intent, getStringExtra,
                                                     env->NewStringUTF("demo_id"));

    if (jDemoId) {
        const char* demoIdChars = env->GetStringUTFChars(jDemoId, nullptr);
        demo_id_out = demoIdChars;
        env->ReleaseStringUTFChars(jDemoId, demoIdChars);
        env->DeleteLocalRef(jDemoId);
    } else {
        demo_id_out.clear();
    }

    env->DeleteLocalRef(intentClass);
    env->DeleteLocalRef(intent);
    env->DeleteLocalRef(activityClass);
}

// ---------------- Android App 指令處理 ----------------
static void handle_cmd(android_app *pApp, int32_t cmd) {
    switch (cmd) {
        case APP_CMD_INIT_WINDOW:
            g_appWindow = pApp->window;
            LOGI("APP_CMD_INIT_WINDOW: window=%p", g_appWindow);

            if (!g_pendingDemoId.empty()) {
                std::string id = g_pendingDemoId;
                g_pendingDemoId.clear();
                selectDemo(id);
            }
            break;

        case APP_CMD_TERM_WINDOW:
            LOGI("APP_CMD_TERM_WINDOW");
            if (currentDemo) {
                currentDemo->destroy();
                currentDemo.reset();
            }
            g_appWindow = nullptr;
            break;

        default:
            break;
    }
}

// ---------------- 觸控與搖桿事件過濾 ----------------
bool motion_event_filter_func(const GameActivityMotionEvent *motionEvent) {
    auto sourceClass = motionEvent->source & AINPUT_SOURCE_CLASS_MASK;
    return (sourceClass == AINPUT_SOURCE_CLASS_POINTER ||
            sourceClass == AINPUT_SOURCE_CLASS_JOYSTICK);
}

// ---------------- 主入口 ----------------
extern "C" void android_main(struct android_app *pApp) {
    LOGI("Welcome to VKDemo");

    pApp->onAppCmd = handle_cmd;
    android_app_set_motion_event_filter(pApp, motion_event_filter_func);

    std::string demo_id;
    get_demo_id_from_intent(pApp, demo_id);
    LOGI("demo_id = %s", demo_id.c_str());

    if (!demo_id.empty()) {
        selectDemo(demo_id);
    }

    while (true) {
        int events;
        android_poll_source* source;
        while (ALooper_pollOnce(0, nullptr, &events, (void**)&source) >= 0) {
            if (source) source->process(pApp, source);
            if (pApp->destroyRequested) return;
        }

        if (currentDemo) {
            currentDemo->render();
        }
    }
}
