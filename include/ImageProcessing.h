#pragma once
#include <cstdint>
#include <vector>

float* convolveImageKernel(float *image, int width, int height, std::vector<std::vector<float>> kernel);
float* harrisCornerDetector(float *image, int width, int height, int blockSize, float sensitivity);
void shiTomasiCornerDetector();
float* threshold(float *image, int width, int height, float threshold);
uint8_t* convertImageTo8bit(float *image, int width, int height);