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

std::vector<float> calculateCovarianceMatrix(const std::vector<float> &image, int width, int height, int blockSize) {
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
    
    std::vector<float> output(3 * width * height);
    for (int i = 0; i < width * height; i++) {
        output[3 * i] = Ix2[i];
        output[3 * i + 1] = IxIy[i];
        output[3 * i + 2] = Iy2[i];
    }

    return output;
}

std::vector<float> harrisCornerDetector(const std::vector<float> &image, int width, int height, int blockSize, float sensitivity) {
    std::vector<float> output(width * height);
    std::vector<float> cov = calculateCovarianceMatrix(image, width, height, blockSize);

    for (int i = 0; i < width * height; i++) {
        const float Ix2 = cov[3 * i];
        const float IxIy = cov[3 * i + 1];
        const float Iy2 = cov[3 * i + 2];

        // Harris Criterion det(M) - k * trace^2(M)
        const float determinant = Ix2 * Iy2 - IxIy * IxIy;
        const float trace = Ix2 + Iy2;
        output[i] = determinant - sensitivity * trace * trace;
    }

    return output;
}

std::vector<float> shiTomasiCornerDetector(const std::vector<float> &image, int width, int height, int blockSize) {
    std::vector<float> output(width * height);
    std::vector<float> cov = calculateCovarianceMatrix(image, width, height, blockSize);

    for (int i = 0; i < width * height; i++) {
        const float Ix2 = cov[3 * i];
        const float IxIy = cov[3 * i + 1];
        const float Iy2 = cov[3 * i + 2];
        
        const float determinant = Ix2 * Iy2 - IxIy * IxIy;
        const float trace = Ix2 + Iy2;

        // Calculate Eigenvalues using algebraic formula
        const float discriminant = std::sqrtf(trace * trace - (4.0f * determinant));
        const float eigenV1 = trace / 2.0f + discriminant;
        const float eigenV2 = trace / 2.0f - discriminant;

        output[i] = std::min(eigenV1, eigenV2);
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
            output[x + y * width] = image[x + y * width];
        exit:
        }
    }
    
    return output;
}

std::vector<Corner> goodFeaturesToTrack(const std::vector<float> &image, int width, int height, float qualityLevel, float minimumDistance) {
    std::vector<float> response = shiTomasiCornerDetector(image, width, height, 2);
    std::vector<float> thresholded = threshold(response, width, height, qualityLevel);
    std::vector<float> nms = nonMaximalSuppression(thresholded, width, height, 3);

    std::vector<Corner> corners;
    // Get response and location of all corners
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (nms[x + y * width] > 0) corners.push_back({nms[x + y * width], x, y});
        }
    }

    // Sort corners by strongest response
    std::sort(corners.begin(), corners.end(), std::greater<Corner>());

    const float sqMinDist = minimumDistance * minimumDistance;
    // Reject corners within minimum distance
    for (int i = 0; i < corners.size(); i++) {
        for (int j = i + 1; j < corners.size(); j++) {
            const int xDist = corners[i].x - corners[j].x;
            const int yDist = corners[i].y - corners[j].y;
            const float sqDist = static_cast<float>(xDist * xDist + yDist * yDist);

            if (sqDist < sqMinDist) {
                corners.erase(corners.begin() + j);
                j--;
            }
        }
    }

    return corners;
}

std::vector<uint8_t> convertImageTo8bit(const std::vector<float> &image, int width, int height, int channels, float gamma) {
    const int size = width * height * channels;
    std::vector<uint8_t> output(size);
    
    // Build Gamma LUT if first time or gamma changes
    static uint8_t gammaLUT[256];
    static float lastGamma = -1.0f;
    if (lastGamma != gamma) {
        const float invGamma = 1.0f / gamma;
        for (int i = 0; i < 256; i++) {
        const float corrected = std::pow(i / 255.0f, invGamma);
            // To round faster, add 0.5f as we know 'corrected' is >= 0
            gammaLUT[i] = static_cast<uint8_t>(corrected * 255.0f + 0.5f);
        }
        lastGamma = gamma;
    }
    
    // Find min/max pixel values
    const auto [minIt, maxIt] = std::minmax_element(image.begin(), image.end());
    const float minimum = *minIt;
    const float maximum = *maxIt;
    // TODO Handle case where image is a solid color (maximum == minimum)

    const float invRange = 1.0f / (maximum - minimum);
    
    for (int i = 0; i < size; i++) {
        // Normalize to  a range of 0 - 1
        float normalized = (image[i] - minimum) * invRange;
        // Clamp to range of 0 - 1 incase of rounding error
        normalized = std::clamp(normalized, 0.0f, 1.0f);
        // Convert to an 8-bit value using gamma LUT
        output[i] = gammaLUT[static_cast<int>(normalized * 255.0f + 0.5f)];
    }

    return output;
}   