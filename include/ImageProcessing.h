#pragma once
#include <cstdint>
#include <vector>
#include <algorithm>

std::vector<float> convolveImageKernel(const std::vector<float> &image, int width, int height, std::vector<std::vector<float>> kernel);
std::vector<float> boxFilter(const std::vector<float> &image, int width, int height, int boxSize);
std::vector<float> harrisCornerDetector(const std::vector<float> &image, int width, int height, int blockSize, float sensitivity);
void shiTomasiCornerDetector();
std::vector<float> threshold(const std::vector<float> &image, int width, int height, float threshold);
std::vector<float> nonMaximalSuppression(const std::vector<float> &image, int width, int height, int blockSize);
uint8_t* convertImageTo8bit(const std::vector<float> &image, int width, int height);