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

uint8_t* harrisCornerDetector(uint8_t *image, int width, int height, int blockSize, float sensitivity) {
    std::vector<std::vector<int>> kernelX = {
        {-1, 0, 1},
        {-2, 0, 2},
        {-1, 0, 1}
    };

    std::vector<std::vector<int>> kernelY = {
        {1, 2, 1},
        {0, 0, 0},
        {-1, -2, -1}
    };

    // 1. Calculate Image gradients in x and y direction
    uint8_t *gradientX = convolveImageKernel(image, width, height, kernelX);
    uint8_t *gradientY = convolveImageKernel(image, width, height, kernelY);

    // 2. Subtract the mean from each image gradient
    /*
    int sumX = 0;
    int sumY = 0;
    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            uint8_t *pixelX = gradientX + (i + width * j); 
            uint8_t *pixelY = gradientY + (i + width * j); 
            
            sumX += *pixelX;
            sumY += *pixelY;
        }
    }

    int avgX = sumX / (width * height);
    int avgY = sumY / (width * height);
    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            uint8_t *pixelX = gradientX + (i + width * j); 
            uint8_t *pixelY = gradientY + (i + width * j); 
            
            *pixelX -= avgX;
            *pixelY -= avgY;
        }
    }
    */

    // Calculate corner response for each block
    uint8_t *output = new uint8_t[width * height];
    for (int i = 0; i < height; i += blockSize) {
        for (int j = 0; j < width; j += blockSize) {
            // 3. Compute covariance matrix
            // [ IxIx  IxIy ]
            // [ IyIx  IyIy ]
            int IxIx = 0;
            int IxIy = 0;
            int IyIy = 0;
            // Element wise multiplication
            for (int y = 0; y < blockSize; y++) {
                for (int x = 0; x < blockSize; x++) {
                    const int dx = j + x;
                    const int dy = i + y;
                    const uint8_t *pixelOffsetX = gradientX + (dx + width * dy);
                    const uint8_t *pixelOffsetY = gradientY + (dx + width * dy);
        
                    IxIx += (*pixelOffsetX) * (*pixelOffsetX);
                    IxIy += (*pixelOffsetX) * (*pixelOffsetY);
                    IyIy += (*pixelOffsetY) * (*pixelOffsetY);
                }
            }
        
            // 4. Harris Criterion det(M) - k * trace^2(M)
            const int determinant = IxIx * IyIy - IxIy * IxIy;
            const int trace = IxIx + IyIy;
            const float response = determinant - sensitivity * trace * trace;

            for (int y = 0; y < blockSize; y++) {
                for (int x = 0; x < blockSize; x++) {
                    const int dx = j + x;
                    const int dy = i + y;
                    
                    // TODO fix error, 8 bits not enough to store response
                    uint8_t *outputOffset = output + (dx + width * dy);
                    *outputOffset = response;
                }
            }
        }
    }
    
    delete gradientX;
    delete gradientY;

    return output;
}