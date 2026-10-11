cmake_minimum_required(VERSION 3.10)

project(ArmorDetector)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)

include_directories(
    include
    ${OpenCV_INCLUDE_DIRS}
)

add_executable(
    ArmorDetector
    src/main.cpp
    src/video_capture.cpp
    src/color_detector.cpp
)

target_link_libraries(
    ArmorDetector
    ${OpenCV_LIBS}
)