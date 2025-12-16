#include <iostream>
#include <vector>
#include "stb_image.h"
#include "stb_image_write.h"
#include "ImageProcessing.h"

int main() {
    int width, height, nChannels;
    // Load image in grayscale
    uint8_t *data = stbi_load("checkerboard.png", &width, &height, &nChannels, STBI_grey);

    if (!data) {
        std::cerr << "Failed to load image" << std::endl;
        return 1;
    }

    std::vector<std::vector<int>> kernelX = {
        {-1, 0, 1},
        {-2, 0, 2},
        {-1, 0, 1}
    };

    uint8_t *gradientF = convolveImageKernel(data, width, height, kernelX);

    uint8_t *response = harrisCornerDetector(data, width, height, 2, 0.04);

    int success = stbi_write_png("response.png", width, height, nChannels, response, width * nChannels);

    if (success) {
        std::cout << "Image saved" << std::endl;
    } else {
        std::cerr << "Failed to save image" << std::endl;
    }

    delete gradientF;
    stbi_image_free(data);

    return 0;
}