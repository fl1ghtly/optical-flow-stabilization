cmake_minimum_required(VERSION 3.24)
project(OpticalFlowStabilization LANGUAGES CXX VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(OpticalFlowLib
    src/ImageProcessing.cpp
    src/stb_image.cpp
    src/stb_image_write.cpp
)

add_subdirectory(apps)

# Include directory for headers
target_include_directories(OpticalFlowLib PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)