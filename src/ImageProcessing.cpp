#include "ImageProcessing.h"

std::vector<double> convolveImageKernel(const std::vector<double> &image, int width, int height, std::vector<std::vector<double>> kernel) {
    std::vector<double>output(width * height);
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

std::vector<double> boxFilter(const std::vector<double> &image, int width, int height, int boxSize, bool normalize) {
    std::vector<double> output(width * height);
    double weight = normalize ? 1.0 / (boxSize * boxSize) : 1.0;
    
    std::vector<std::vector<double>> hKernel(1, std::vector<double>(boxSize, 1.0 / weight));
    output = convolveImageKernel(image, width, height, hKernel);
    std::vector<std::vector<double>> vKernel(boxSize, std::vector<double>(1, 1.0 / weight));
    return convolveImageKernel(output, width, height, vKernel);
}

std::vector<double> calculateCovarianceMatrix(const std::vector<double> &image, int width, int height, int blockSize) {
    static const std::vector<std::vector<double>> kernelX = {
        {-1, 0, 1},
        {-2, 0, 2},
        {-1, 0, 1}
    };
    
    static const std::vector<std::vector<double>> kernelY = {
        {1, 2, 1},
        {0, 0, 0},
        {-1, -2, -1}
    };
    
    // Calculate Image gradients in x and y direction
    std::vector<double> gradientX = convolveImageKernel(image, width, height, kernelX);
    std::vector<double> gradientY = convolveImageKernel(image, width, height, kernelY);
    
    // Compute Covariance matrix for each pixel
    std::vector<double> Ix2(width * height);
    std::vector<double> IxIy(width * height);
    std::vector<double> Iy2(width * height);
    
    for (int i = 0; i < width * height; i++) {
        Ix2[i] = gradientX[i] * gradientX[i];
        IxIy[i] = gradientX[i] * gradientY[i];
        Iy2[i] = gradientY[i] * gradientY[i];
    }
    
    // Multiply covariance matrix with window (box)
    Ix2 = boxFilter(Ix2, width, height, blockSize);
    IxIy = boxFilter(IxIy, width, height, blockSize);
    Iy2 = boxFilter(Iy2, width, height, blockSize);
    
    std::vector<double> output(3 * width * height);
    for (int i = 0; i < width * height; i++) {
        output[3 * i] = Ix2[i];
        output[3 * i + 1] = IxIy[i];
        output[3 * i + 2] = Iy2[i];
    }

    return output;
}

std::vector<double> harrisCornerDetector(const std::vector<double> &image, int width, int height, int blockSize, double sensitivity) {
    std::vector<double> output(width * height);
    std::vector<double> cov = calculateCovarianceMatrix(image, width, height, blockSize);

    for (int i = 0; i < width * height; i++) {
        const double Ix2 = cov[3 * i];
        const double IxIy = cov[3 * i + 1];
        const double Iy2 = cov[3 * i + 2];

        // Harris Criterion det(M) - k * trace^2(M)
        const double determinant = Ix2 * Iy2 - IxIy * IxIy;
        const double trace = Ix2 + Iy2;
        output[i] = determinant - sensitivity * trace * trace;
    }

    return output;
}

std::vector<double> shiTomasiCornerDetector(const std::vector<double> &image, int width, int height, int blockSize) {
    std::vector<double> output(width * height);
    std::vector<double> cov = calculateCovarianceMatrix(image, width, height, blockSize);

    for (int i = 0; i < width * height; i++) {
        const double Ix2 = cov[3 * i];
        const double IxIy = cov[3 * i + 1];
        const double Iy2 = cov[3 * i + 2];
        
        const double determinant = Ix2 * Iy2 - IxIy * IxIy;
        const double trace = Ix2 + Iy2;

        // Calculate Eigenvalues using algebraic formula
        const double discriminant = std::sqrtf(trace * trace - (4.0 * determinant));
        const double eigenV1 = trace / 2.0 + discriminant;
        const double eigenV2 = trace / 2.0 - discriminant;

        output[i] = std::min(eigenV1, eigenV2);
    }

    return output;
}

std::vector<double> threshold(const std::vector<double> &image, int width, int height, double threshold) {
    std::vector<double>output(width * height);
    const double maxVal = *std::max_element(image.begin(), image.end());

    for (int i = 0; i < width * height; i++) {
        if (image[i] >= threshold * maxVal) {
            output[i] = image[i];
        }
    }

    return output;
}

std::vector<double> nonMaximalSuppression(const std::vector<double> &image, int width, int height, int blockSize) {
    std::vector<double>output(width * height);

    // Check for every pixel
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            double pixelValue = image[x + y * width];
            
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

std::vector<Point> goodFeaturesToTrack(const std::vector<double> &image, int width, int height, double qualityLevel, double minimumDistance) {
    std::vector<double> response = shiTomasiCornerDetector(image, width, height, 2);
    std::vector<double> thresholded = threshold(response, width, height, qualityLevel);
    std::vector<double> nms = nonMaximalSuppression(thresholded, width, height, 3);

    std::vector<std::pair<double, Point>> corners;
    // Get response and location of all corners
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (nms[x + y * width] > 0) corners.push_back({nms[x + y * width], {x, y}});
        }
    }

    // Sort corners by strongest response
    std::sort(corners.begin(), corners.end(), std::greater<std::pair<double, Point>>());

    // Remove response data from feature list
    std::vector<Point> features;
    features.reserve(corners.size());

    for (auto& p : corners) {
        features.push_back(p.second);
    }

    const double sqMinDist = minimumDistance * minimumDistance;
    // Reject features within minimum distance
    for (int i = 0; i < features.size(); i++) {
        for (int j = i + 1; j < features.size(); j++) {
            const int xDist = features[i].x - features[j].x;
            const int yDist = features[i].y - features[j].y;
            const double sqDist = static_cast<double>(xDist * xDist + yDist * yDist);

            if (sqDist < sqMinDist) {
                features.erase(features.begin() + j);
                j--;
            }
        }
    }

    return features;
}

std::vector<uint8_t> convertImageTo8bit(const std::vector<double> &image, int width, int height, int channels, double gamma) {
    const int size = width * height * channels;
    std::vector<uint8_t> output(size);
    
    // Build Gamma LUT if first time or gamma changes
    static uint8_t gammaLUT[256];
    static double lastGamma = -1.0;
    if (lastGamma != gamma) {
        const double invGamma = 1.0 / gamma;
        for (int i = 0; i < 256; i++) {
        const double corrected = std::pow(i / 255.0, invGamma);
            // To round faster, add 0.5f as we know 'corrected' is >= 0
            gammaLUT[i] = static_cast<uint8_t>(corrected * 255.0 + 0.5);
        }
        lastGamma = gamma;
    }
    
    // Find min/max pixel values
    const auto [minIt, maxIt] = std::minmax_element(image.begin(), image.end());
    const double minimum = *minIt;
    const double maximum = *maxIt;
    // TODO Handle case where image is a solid color (maximum == minimum)

    const double invRange = 1.0 / (maximum - minimum);
    
    for (int i = 0; i < size; i++) {
        // Normalize to  a range of 0 - 1
        double normalized = (image[i] - minimum) * invRange;
        // Clamp to range of 0 - 1 incase of rounding error
        normalized = std::clamp(normalized, 0.0, 1.0);
        // Convert to an 8-bit value using gamma LUT
        output[i] = gammaLUT[static_cast<int>(normalized * 255.0 + 0.5)];
    }

    return output;
}   

    }

    return output;
}   