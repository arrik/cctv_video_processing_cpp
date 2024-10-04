cmake_minimum_required(VERSION 3.27)
project(cctv_video_processing_cpp)

set(CMAKE_CXX_STANDARD 17)
set(OPENCV_FFMPEG_USE_FIND_PACKAGE=ON)

find_package( OpenCV REQUIRED )

add_executable(cctv_video_processing_cpp src/main.cpp)
target_include_directories(cctv_video_processing_cpp PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(cctv_video_processing_cpp ${OpenCV_LIBS})