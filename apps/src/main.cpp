#include <iostream>
#include <vector>
#include "stb_image.h"
#include "stb_image_write.h"
#include "ImageProcessing.h"

int main() {
    int width, height, nChannels;
    // Load image in grayscale
    float *data = stbi_loadf("checkerboard.png", &width, &height, &nChannels, STBI_grey);
    // nChannels is set to the amount of channels in the original image, need to set to 1 for grayscale
    nChannels = 1;
    
    if (!data) {
        std::cerr << "Failed to load image" << std::endl;
        return 1;
    }

    std::vector<float> vec(data, data + width * height * nChannels);
    stbi_image_free(data);

    std::vector<float> response = harrisCornerDetector(vec, width, height, 2, 0.04);
    std::vector<float> thresholded = threshold(response, width, height, 0.01);
    std::vector<float> nms = nonMaximalSuppression(thresholded, width, height, 5);
    uint8_t *output = convertImageTo8bit(nms, width, height);
    
    int success = stbi_write_png("output.png", width, height, nChannels, output, width * nChannels);
    // TODO NMS
    if (success) {
        std::cout << "Image saved" << std::endl;
    } else {
        std::cerr << "Failed to save image" << std::endl;
    }

    delete[] output;

    return 0;
}