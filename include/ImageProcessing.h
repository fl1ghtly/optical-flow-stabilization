#pragma once
#include <cstdint>
#include <vector>
#include <algorithm>
#include <cmath>
#include <random>
#include <Eigen/Dense>

struct Vector2f {
	float x;
	float y;

	friend bool operator<(const Vector2f& lhs, const Vector2f& rhs) { return lhs.x < rhs.x ? true : lhs.y < rhs.y; }
	friend bool operator>(const Vector2f& lhs, const Vector2f& rhs) { return rhs < lhs; };
	friend bool operator<=(const Vector2f& lhs, const Vector2f& rhs) { return !(lhs > rhs); };
	friend bool operator>=(const Vector2f& lhs, const Vector2f& rhs) { return !(lhs < rhs); };
};

std::vector<double> convolveImageKernel(const std::vector<double> &image, int width, int height, int channels, std::vector<std::vector<double>> kernel);
std::vector<double> sobel(const std::vector<double> &image, int width, int height);
std::vector<double> boxFilter(const std::vector<double> &image, int width, int height, int channels, int boxSize, bool normalize=false);
std::vector<double> gaussianPyramid(const std::vector<double> &image, int width, int height, int channels);
std::vector<double> calculateCovarianceMatrix(const std::vector<double> &image, int width, int height, int blockSize);
std::vector<double> harrisCornerDetector(const std::vector<double> &image, int width, int height, int blockSize, double sensitivity);
std::vector<double> shiTomasiCornerDetector(const std::vector<double> &image, int width, int height, int blockSize);
std::vector<double> threshold(const std::vector<double> &image, int width, int height, double threshold);
std::vector<double> nonMaximalSuppression(const std::vector<double> &image, int width, int height, int blockSize);
std::vector<Vector2f> goodFeaturesToTrack(const std::vector<double> &image, int width, int height, double qualityLevel, double minimumDistance);
std::vector<uint8_t> convertImageTo8bit(const std::vector<double> &image, int width, int height, int channels, double gamma=2.2f);
std::vector<Vector2f> lucasKanadeOpticalFlow(const std::vector<double> &prev, const std::vector<double> &next, int width, int height, const std::vector<Vector2f> &features, int windowSize);
std::vector<Vector2f> lucasKanadeOpticalFlowPyramid(const std::vector<double> &prev, const std::vector<double> &next, int width, int height, int levels, const std::vector<Vector2f> &features, int windowSize);
Eigen::Matrix<double, 2, 3> estimateAffineTransform(const std::vector<Vector2f> &prevPts, const std::vector<Vector2f> &nextPts, float reprojectionThreshold);