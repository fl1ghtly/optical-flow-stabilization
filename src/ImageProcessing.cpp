#include "ImageProcessing.h"

uint8_t* convolveImageKernel(uint8_t *image, int width, int height, std::vector<std::vector<int>> kernel) {
    uint8_t *output = new uint8_t[width * height];
    // Convolve the kernel at each pixel
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            uint8_t *outputPixel = output + (x + width * y);
            *outputPixel = 0;
            for (int j = 0; j < kernel.size(); j++) {
                for (int i = 0; i < kernel[0].size(); i++) {
                    // Get the address of the (x + dx, y + dy) pixel 
                    const int dx = x + (i - kernel[0].size() / 2);
                    const int dy = y + (j - kernel.size() / 2);
                    
                    // Pixels beyond image edges are treated as '0'
                    if (dx < 0 || dx >= width || dy < 0 || dy >= height) continue;
                    uint8_t *pixelOffset = image + (dx + width * dy);
                    *outputPixel += (*pixelOffset) * kernel[j][i];
                }
            }
        }
    }
    return output;
}