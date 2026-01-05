#include <iostream>
#include <vector>
#include "stb_image.h"
#include "stb_image_write.h"
#include "ImageProcessing.h"

int main() {
    int width, height, nChannels;
    // Load image in grayscale
    float *data = stbi_loadf("data.png", &width, &height, &nChannels, STBI_grey);
    float *dataNext = stbi_loadf("data_next.png", &width, &height, &nChannels, STBI_grey);
    // nChannels is set to the amount of channels in the original image, need to set to 1 for grayscale
    nChannels = 1;
    
    if (!data || !dataNext) {
        std::cerr << "Failed to load image" << std::endl;
        return 1;
    }

    std::vector<double> prev(data, data + width * height * nChannels);
    std::vector<double> next(dataNext, dataNext + width * height * nChannels);
    stbi_image_free(data);
    stbi_image_free(dataNext);

    auto prevPts = goodFeaturesToTrack(prev, width, height, 0.01, 10.0);
    auto nextPts = lucasKanadeOpticalFlow(prev, next, width, height, prevPts, 25);
    
    auto transformation = estimateAffineTransform(prevPts, nextPts, 5.0f);
    // auto nextPts = lucasKanadeOpticalFlowPyramid(prev, next, width, height, 3, prevPts, 25);

    for (int i = 0; i < nextPts.size(); i++) {
        std::cout << nextPts[i].x - prevPts[i].x << ", " << nextPts[i].y - prevPts[i].y << std::endl;
    }

    return 0;
}