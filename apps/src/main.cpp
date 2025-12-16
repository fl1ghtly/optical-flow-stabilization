#include <iostream>
#include <vector>
#include "stb_image.h"
#include "stb_image_write.h"
#include "ImageProcessing.h"

int main() {
    int width, height, nChannels;
    // Load image in grayscale
    float *data = stbi_loadf("checkerboard.png", &width, &height, &nChannels, STBI_grey);

    if (!data) {
        std::cerr << "Failed to load image" << std::endl;
        return 1;
    }

    std::vector<std::vector<int>> kernelX = {
        {-1, 0, 1},
        {-2, 0, 2},
        {-1, 0, 1}
    };

    float *gradientF = convolveImageKernel(data, width, height, kernelX);

    float *response = harrisCornerDetector(data, width, height, 2, 0.04);

    int success = stbi_write_png("data.png", width, height, nChannels, data, width * nChannels);

    if (success) {
        std::cout << "Image saved" << std::endl;
    } else {
        std::cerr << "Failed to save image" << std::endl;
    }

    delete response;
    stbi_image_free(data);

    return 0;
}