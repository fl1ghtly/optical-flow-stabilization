#include "ImageProcessing.h"

float* convolveImageKernel(float *image, int width, int height, std::vector<std::vector<int>> kernel) {
    float *output = new float[width * height];
    // Convolve the kernel at each pixel
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float *outputPixel = output + (x + width * y);
            *outputPixel = 0;
            for (int j = 0; j < kernel.size(); j++) {
                for (int i = 0; i < kernel[0].size(); i++) {
                    // Get the address of the (x + dx, y + dy) pixel 
                    const int dx = x + (i - kernel[0].size() / 2);
                    const int dy = y + (j - kernel.size() / 2);
                    
                    // Pixels beyond image edges are treated as '0'
                    if (dx < 0 || dx >= width || dy < 0 || dy >= height) continue;
                    float *pixelOffset = image + (dx + width * dy);
                    *outputPixel += (*pixelOffset) * kernel[j][i];
                }
            }
        }
    }
    return output;
}

float* harrisCornerDetector(float *image, int width, int height, int blockSize, float sensitivity) {
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

    // Calculate Image gradients in x and y direction
    float *gradientX = convolveImageKernel(image, width, height, kernelX);
    float *gradientY = convolveImageKernel(image, width, height, kernelY);

    // Calculate corner response for each block
    float *output = new float[width * height];
    for (int i = 0; i < height; i += blockSize) {
        for (int j = 0; j < width; j += blockSize) {
            // Compute covariance matrix
            // [ IxIx  IxIy ]
            // [ IyIx  IyIy ]
            float IxIx = 0.f;
            float IxIy = 0.f;
            float IyIy = 0.f;
            // Element wise multiplication
            for (int y = 0; y < blockSize; y++) {
                for (int x = 0; x < blockSize; x++) {
                    const int dx = j + x;
                    const int dy = i + y;
                    if (dx >= width || dy >= height) continue;
                    const float *pixelOffsetX = gradientX + (dx + width * dy);
                    const float *pixelOffsetY = gradientY + (dx + width * dy);
        
                    IxIx += (*pixelOffsetX) * (*pixelOffsetX);
                    IxIy += (*pixelOffsetX) * (*pixelOffsetY);
                    IyIy += (*pixelOffsetY) * (*pixelOffsetY);
                }
            }
        
            // Harris Criterion det(M) - k * trace^2(M)
            const int determinant = IxIx * IyIy - IxIy * IxIy;
            const int trace = IxIx + IyIy;
            float response = determinant - sensitivity * trace * trace;
            // Edges (R < 0) and flat regions (R ~ 0) should be clamped to 0 (without consequence to corner detection) 
            // to avoid errors when converting to 8-bit
            response = std::max(0.f, response);

            for (int y = 0; y < blockSize; y++) {
                for (int x = 0; x < blockSize; x++) {
                    const int dx = j + x;
                    const int dy = i + y;
                    if (dx >= width || dy >= height) continue;
                    
                    float *outputOffset = output + (dx + width * dy);
                    *outputOffset = response;
                }
            }
        }
    }
    
    delete[] gradientX;
    delete[] gradientY;

    return output;
}

uint8_t* convertImageTo8bit(float *image, int width, int height) {
    uint8_t *output = new uint8_t[width * height];

    float maximum = image[0];
    float minimum = image[0];
    for (int i = 0; i < width * height; i++) {
        maximum = image[i] > maximum ? image[i] : maximum;
        minimum = image[i] < minimum ? image[i] : minimum;
    }

    float range = maximum - minimum;

    for (int i = 0; i < width * height; i++) {
        // Normalize to  a range of 0 - 1
        float normalized = (image[i] - minimum) / range;
        // Clamp to range of 0 - 1 incase of rounding error
        normalized = normalized < 0.f ? 0.f : (normalized > 1.f ? 1.f : normalized);
        // Convert to an 8-bit value
        output[i] = static_cast<uint8_t>(normalized * 255.f);
    }

    return output;
}   