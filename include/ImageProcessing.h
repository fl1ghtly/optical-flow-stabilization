#pragma once
#include <cstdint>
#include <vector>

uint8_t* convolveImageKernel(uint8_t *image, int width, int height, std::vector<std::vector<int>> kernel);

void harrisCornerDetector();
void shiTomasiCornerDetector();