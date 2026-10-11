cmake_minimum_required(VERSION 3.16)

project(ArmorDetector LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(OpenCV_DIR /usr/local/lib/cmake/opencv4)

find_package(OpenCV 4.6.0 REQUIRED)


add_executable(ArmorDetector
    src/main.cpp
    src/video_capture.cpp
)


target_include_directories(ArmorDetector
    PRIVATE
    include
)


target_link_libraries(ArmorDetector
    PRIVATE
    ${OpenCV_LIBS}
)
