#include "ImageProcessing.h"

std::vector<float> convolveImageKernel(const std::vector<float> &image, int width, int height, std::vector<std::vector<float>> kernel) {
    std::vector<float>output(width * height);
    // Convolve the kernel at each pixel
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            for (int j = 0; j < kernel.size(); j++) {
                int dy = y + (j - kernel.size() / 2);
                // Pixels beyond image edges should replicate the nearest valid pixel
                dy = std::clamp(dy, 0, height - 1);

                for (int i = 0; i < kernel[0].size(); i++) {
                    int dx = x + (i - kernel[0].size() / 2);
                    dx = std::clamp(dx, 0, width - 1);

                    output[x + y * width] += image[dx + dy * width] * kernel[j][i];
                }
            }
        }
    }
    return output;
}

std::vector<float> boxFilter(const std::vector<float> &image, int width, int height, int boxSize, bool normalize) {
    std::vector<float> output(width * height);
    float weight = normalize ? 1.0f / (boxSize * boxSize) : 1.0f;
    
    std::vector<std::vector<float>> hKernel(1, std::vector<float>(boxSize, 1.0f / weight));
    output = convolveImageKernel(image, width, height, hKernel);
    std::vector<std::vector<float>> vKernel(boxSize, std::vector<float>(1, 1.0f / weight));
    return convolveImageKernel(output, width, height, vKernel);
}

std::vector<float> harrisCornerDetector(const std::vector<float> &image, int width, int height, int blockSize, float sensitivity) {
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
    std::vector<float> gradientX = convolveImageKernel(image, width, height, kernelX);
    std::vector<float> gradientY = convolveImageKernel(image, width, height, kernelY);

    // Compute Covariance matrix for each pixel
    std::vector<float> Ix2(width * height);
    std::vector<float> IxIy(width * height);
    std::vector<float> Iy2(width * height);
    
    for (int i = 0; i < width * height; i++) {
        Ix2[i] = gradientX[i] * gradientX[i];
        IxIy[i] = gradientX[i] * gradientY[i];
        Iy2[i] = gradientY[i] * gradientY[i];
    }

    // Multiply covariance matrix with window (box)
    Ix2 = boxFilter(Ix2, width, height, blockSize);
    IxIy = boxFilter(IxIy, width, height, blockSize);
    Iy2 = boxFilter(Iy2, width, height, blockSize);

    std::vector<float>output(width * height);

    for (int i = 0; i < width * height; i++) {
        // Harris Criterion det(M) - k * trace^2(M)
        const float determinant = Ix2[i] * Iy2[i] - IxIy[i] * IxIy[i];
        const float trace = Ix2[i] + Iy2[i];
        const float response = determinant - sensitivity * trace * trace;
        output[i] = response;
    }

    return output;
}

std::vector<float> threshold(const std::vector<float> &image, int width, int height, float threshold) {
    std::vector<float>output(width * height);
    const float maxVal = *std::max_element(image.begin(), image.end());

    for (int i = 0; i < width * height; i++) {
        if (image[i] >= threshold * maxVal) {
            output[i] = image[i];
        }
    }

    return output;
}

std::vector<float> nonMaximalSuppression(const std::vector<float> &image, int width, int height, int blockSize) {
    std::vector<float>output(width * height);

    // Check for every pixel
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float pixelValue = image[x + y * width];
            
            // Skip pixel if 0
            if (pixelValue == 0) continue;

            // Check each pixel's neighbors, if any are greater than this pixel, suppress the pixel and move on
            for (int i = 0; i < blockSize; i++) {
                const int dy = y + (i - blockSize / 2);
                if (dy < 0 || dy >= height) continue;

                for (int j = 0; j < blockSize; j++) {
                    const int dx = x + (j - blockSize / 2);
                    if (dx < 0 || dx >= width) continue;

                    // Found another neighbor greater than current pixel, therefore stop searching and go to next pixel
                    if (image[dx + dy * width] > pixelValue) goto exit;
                }
            }
            // Otherwise this is the maximum pixel in the block
            output[x + y * width] = 1.0f;
        exit:
        }
    }
    
    return output;
}

uint8_t* convertImageTo8bit(const std::vector<float> &image, int width, int height) {
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