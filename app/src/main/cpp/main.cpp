#include <jni.h>

#include "AndroidOut.h"
#include "Renderer.h"

#include <game-activity/GameActivity.cpp>
#include <game-text-input/gametextinput.cpp>

#include "DemoRegistry.hpp"

extern "C" {

#include <game-activity/native_app_glue/android_native_app_glue.c>

/*!
 * Handles commands sent to this Android application
 * @param pApp the app the commands are coming from
 * @param cmd the command to handle
 */
void handle_cmd(android_app *pApp, int32_t cmd) {
    switch (cmd) {
        case APP_CMD_INIT_WINDOW:
            // A new window is created, associate a renderer with it. You may replace this with a
            // "game" class if that suits your needs. Remember to change all instances of userData
            // if you change the class here as a reinterpret_cast is dangerous this in the
            // android_main function and the APP_CMD_TERM_WINDOW handler case.
            pApp->userData = new Renderer(pApp);
            break;
        case APP_CMD_TERM_WINDOW:
            // The window is being destroyed. Use this to clean up your userData to avoid leaking
            // resources.
            //
            // We have to check if userData is assigned just in case this comes in really quickly
            if (pApp->userData) {
                //
                auto *pRenderer = reinterpret_cast<Renderer *>(pApp->userData);
                pApp->userData = nullptr;
                delete pRenderer;
            }
            break;
        default:
            break;
    }
}

/*!
 * Enable the motion events you want to handle; not handled events are
 * passed back to OS for further processing. For this example case,
 * only pointer and joystick devices are enabled.
 *
 * @param motionEvent the newly arrived GameActivityMotionEvent.
 * @return true if the event is from a pointer or joystick device,
 *         false for all other input devices.
 */
bool motion_event_filter_func(const GameActivityMotionEvent *motionEvent) {
    auto sourceClass = motionEvent->source & AINPUT_SOURCE_CLASS_MASK;
    return (sourceClass == AINPUT_SOURCE_CLASS_POINTER ||
            sourceClass == AINPUT_SOURCE_CLASS_JOYSTICK);
}

// 只要取得 pApp->activity, 就能存取 Java Activity instance
void get_demo_id_from_intent(android_app* pApp, std::string& demo_id_out) {
    JNIEnv* env = nullptr;
    pApp->activity->vm->AttachCurrentThread(&env, nullptr);

    GameActivity* ga = pApp->activity;
    jobject javaActivity = ga->javaGameActivity;

    jclass activityClass = env->GetObjectClass(javaActivity);
    jmethodID getIntent = env->GetMethodID(activityClass, "getIntent", "()Landroid/content/Intent;");
    jobject intent = env->CallObjectMethod(javaActivity, getIntent);

    jclass intentClass = env->GetObjectClass(intent);
    jmethodID getStringExtra = env->GetMethodID(intentClass, "getStringExtra", "(Ljava/lang/String;)Ljava/lang/String;");
    jstring jDemoId = (jstring)env->CallObjectMethod(intent, getStringExtra, env->NewStringUTF("demo_id"));

    if (jDemoId) {
        const char* demoIdChars = env->GetStringUTFChars(jDemoId, nullptr);
        demo_id_out = demoIdChars;
        env->ReleaseStringUTFChars(jDemoId, demoIdChars);
        env->DeleteLocalRef(jDemoId);
    } else {
        demo_id_out = "";
    }

    env->DeleteLocalRef(intentClass);
    env->DeleteLocalRef(intent);
    env->DeleteLocalRef(activityClass);
    // 不要 DetachCurrentThread!
}

void finishGameActivity(android_app* app) {
    JNIEnv* env = nullptr;
    app->activity->vm->AttachCurrentThread(&env, nullptr);

    GameActivity* ga = app->activity;
    jobject javaActivity = ga->javaGameActivity;

    jclass activityClass = env->GetObjectClass(javaActivity);
    jmethodID finishMethod = env->GetMethodID(activityClass, "finish", "()V");
    env->CallVoidMethod(javaActivity, finishMethod);

    env->DeleteLocalRef(activityClass);
    // 不要 detach
}

std::unique_ptr<Demo> currentDemo;

void selectDemo(const std::string& demo_id) {
    auto it = demoRegistry.find(demo_id);
    if (it != demoRegistry.end()) {
        if (currentDemo) currentDemo->destroy();
        currentDemo = it->second();
        currentDemo->init();
    }
}

/*!
 * This the main entry point for a native activity
 */
void android_main(struct android_app *pApp) {
    // Can be removed, useful to ensure your code is running
    aout << "Welcome to android_main" << std::endl;

    std::string demo_id;
    get_demo_id_from_intent(pApp, demo_id);
    aout << "demo_id from intent: " << demo_id << std::endl;
    // 用 demo_id 決定要啟動哪個 Demo
    selectDemo(demo_id);
    if (currentDemo) {
        aout << "Selected demo: " << currentDemo->getName() << std::endl;
    } else {
        aout << "No demo selected (demo_id: " << demo_id << ")" << std::endl;
    }

    // Register an event handler for Android events
    pApp->onAppCmd = handle_cmd;

    // Set input event filters (set it to NULL if the app wants to process all inputs).
    // Note that for key inputs, this example uses the default default_key_filter()
    // implemented in android_native_app_glue.c.
    android_app_set_motion_event_filter(pApp, motion_event_filter_func);

    // This sets up a typical game/event loop. It will run until the app is destroyed.
    do {
        // Process all pending events before running game logic.
        bool done = false;
        while (!done) {
            // 0 is non-blocking.
            int timeout = 0;
            int events;
            android_poll_source *pSource;
            int result = ALooper_pollOnce(timeout, nullptr, &events,
                                          reinterpret_cast<void**>(&pSource));
            switch (result) {
                case ALOOPER_POLL_TIMEOUT:
                    [[clang::fallthrough]];
                case ALOOPER_POLL_WAKE:
                    // No events occurred before the timeout or explicit wake. Stop checking for events.
                    done = true;
                    break;
                case ALOOPER_EVENT_ERROR:
                    aout << "ALooper_pollOnce returned an error" << std::endl;
                    break;
                case ALOOPER_POLL_CALLBACK:
                    break;
                default:
                    if (pSource) {
                        pSource->process(pApp, pSource);
                    }
            }
        }

        // Check if any user data is associated. This is assigned in handle_cmd
        if (pApp->userData) {
            // We know that our user data is a Renderer, so reinterpret cast it. If you change your
            // user data remember to change it here
            auto *pRenderer = reinterpret_cast<Renderer *>(pApp->userData);

            // Process game input
            pRenderer->handleInput();

            // Render a frame
            pRenderer->render();
        }
    } while (!pApp->destroyRequested);
}
}