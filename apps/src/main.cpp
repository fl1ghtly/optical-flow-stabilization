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

    std::vector<std::vector<int>> kernel = {
        {-1, 0, 1},
        {-2, 0, 2},
        {-1, 0, 1}
    };

    uint8_t *gradient = convolveImageKernel(data, width, height, kernel);

    int success = stbi_write_png("gradient.png", width, height, nChannels, gradient, width * nChannels);

    if (success) {
        std::cout << "Image saved" << std::endl;
    } else {
        std::cerr << "Failed to save image" << std::endl;
    }

    delete gradient;
    stbi_image_free(data);

    return 0;
}