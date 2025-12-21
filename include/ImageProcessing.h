#pragma once
#include <cstdint>
#include <vector>

float* convolveImageKernel(float *image, int width, int height, std::vector<std::vector<float>> kernel);
float* harrisCornerDetector(float *image, int width, int height, int blockSize, float sensitivity);
void shiTomasiCornerDetector();

uint8_t* convertImageTo8bit(float *image, int width, int height);