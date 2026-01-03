cmake_minimum_required(VERSION 3.24)
project(OpticalFlowStabilization LANGUAGES CXX VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Eigen3 REQUIRED NO_MODULE)
include_directories(${EIGEN_INCLUDE_DIR})

add_library(OpticalFlowLib
    src/ImageProcessing.cpp
    src/stb_image.cpp
    src/stb_image_write.cpp
)

add_subdirectory(apps)

target_link_libraries(OpticalFlowLib
    Eigen3::Eigen
)

# Include directory for headers
target_include_directories(OpticalFlowLib PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)