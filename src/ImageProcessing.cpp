#include "ImageProcessing.h"

float* convolveImageKernel(float *image, int width, int height, std::vector<std::vector<float>> kernel) {
    float *output = new float[width * height];
    // Convolve the kernel at each pixel
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float *outputPixel = output + (x + width * y);
            *outputPixel = 0;
            for (int j = 0; j < kernel.size(); j++) {
                for (int i = 0; i < kernel[0].size(); i++) {
                    // Get the address of the (x + dx, y + dy) pixel 
                    int dx = x + (i - kernel[0].size() / 2);
                    int dy = y + (j - kernel.size() / 2);
                    
                    // Pixels beyond image edges should replicate the nearest valid pixel
                    dx = std::min(width - 1, std::max(0, dx));
                    dy = std::min(height - 1, std::max(0, dy));

                    float *pixelOffset = image + (dx + width * dy);
                    *outputPixel += (*pixelOffset) * kernel[j][i];
                }
            }
        }
    }
    return output;
}

float* harrisCornerDetector(float *image, int width, int height, int blockSize, float sensitivity) {
    std::vector<std::vector<float>> kernelX = {
        {-1, 0, 1},
        {-2, 0, 2},
        {-1, 0, 1}
    };

    std::vector<std::vector<float>> kernelY = {
        {1, 2, 1},
        {0, 0, 0},
        {-1, -2, -1}
    };

    // Calculate Image gradients in x and y direction
    float *gradientX = convolveImageKernel(image, width, height, kernelX);
    float *gradientY = convolveImageKernel(image, width, height, kernelY);

    // Compute Covariance matrix for each pixel
    std::vector<float> Ix2(width * height);
    std::vector<float> IxIy(width * height);
    std::vector<float> Iy2(width * height);
    
    for (int i = 0; i < width * height; i++) {
        Ix2[i] = gradientX[i] * gradientX[i];
        IxIy[i] = gradientX[i] * gradientY[i];
        Iy2[i] = gradientY[i] * gradientY[i];
    }

    delete[] gradientX;
    delete[] gradientY;
    
    // Multiply covariance matrix with window (Gaussian)
    std::vector<std::vector<float>> gaussianKernel = {
        {1.0/16.0, 1.0/8.0, 1.0/16.0},
        {1.0/8.0, 1.0/4.0, 1.0/8.0},
        {1.0/16.0, 1.0/8.0, 1.0/16.0},
    };

    std::vector<std::vector<float>> boxKernel = {
        {1, 1},
        {1, 1},
    };

    float* gIx2 = convolveImageKernel(Ix2.data(), width, height, boxKernel);
    float* gIxIy = convolveImageKernel(IxIy.data(), width, height, boxKernel);
    float* gIy2 = convolveImageKernel(Iy2.data(), width, height, boxKernel);

    float *output = new float[width * height];

    for (int i = 0; i < width * height; i++) {
        // Harris Criterion det(M) - k * trace^2(M)
        const float determinant = gIx2[i] * gIy2[i] - gIxIy[i] * gIxIy[i];
        const float trace = gIx2[i] + gIy2[i];
        const float response = determinant - sensitivity * trace * trace;
        output[i] = response;
    }

    delete[] gIx2;
    delete[] gIxIy;
    delete[] gIy2;

    return output;
}
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