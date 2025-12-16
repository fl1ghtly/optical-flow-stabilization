#pragma once
#include <cstdint>
#include <iostream>
#include <vector>

float* convolveImageKernel(float *image, int width, int height, std::vector<std::vector<int>> kernel);

float* harrisCornerDetector(float *image, int width, int height, int blockSize, float sensitivity);
void shiTomasiCornerDetector();