# Add executable target
add_executable(StabilizeVideo
    src/main.cpp
)

target_include_directories(StabilizeVideo PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include # Apps/Include
)

# Link for StabilizeVideo target
# Link dependencies only for executable
target_link_libraries(StabilizeVideo PRIVATE
    OpticalFlowLib
)
