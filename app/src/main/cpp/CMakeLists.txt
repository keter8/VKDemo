cmake_minimum_required(VERSION 3.22.1)
project(vkdemo LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(vkdemo SHARED
        main.cpp
        DemoRegistry.cpp
        samples/GLES/ClearGL.cpp
        samples/GLES/RegisterGlesSamples.cpp
)

# 你自己的 include（專案內檔案）
target_include_directories(vkdemo PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/samples/GLES
)

target_link_options(${CMAKE_PROJECT_NAME} PRIVATE "-Wl,-z,max-page-size=16384")

# 從 AGP 的 Prefab 拿到 game-activity
find_package(game-activity REQUIRED CONFIG)

find_library(log-lib     log)
find_library(android-lib android)
find_library(egl-lib     EGL)
find_library(gles-lib    GLESv3)  # 或 GLESv2

target_link_libraries(vkdemo
        game-activity::game-activity
        ${egl-lib}
        ${gles-lib}
        ${android-lib}
        ${log-lib}
        m
)
