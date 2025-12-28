#pragma once
#include <cstdint>
#include <vector>
#include <algorithm>
#include <cmath>

struct Point {
	int x;
	int y;

	friend bool operator<(const Point& lhs, const Point& rhs) { return lhs.x < rhs.x ? true : lhs.y < rhs.y; }
	friend bool operator>(const Point& lhs, const Point& rhs) { return rhs < lhs; };
	friend bool operator<=(const Point& lhs, const Point& rhs) { return !(lhs > rhs); };
	friend bool operator>=(const Point& lhs, const Point& rhs) { return !(lhs < rhs); };
};

std::vector<double> convolveImageKernel(const std::vector<double> &image, int width, int height, std::vector<std::vector<double>> kernel);
std::vector<double> sobel(const std::vector<double> &image, int width, int height);
std::vector<double> boxFilter(const std::vector<double> &image, int width, int height, int boxSize, bool normalize=false);
std::vector<double> calculateCovarianceMatrix(const std::vector<double> &image, int width, int height, int blockSize);
std::vector<double> harrisCornerDetector(const std::vector<double> &image, int width, int height, int blockSize, double sensitivity);
std::vector<double> shiTomasiCornerDetector(const std::vector<double> &image, int width, int height, int blockSize);
std::vector<double> threshold(const std::vector<double> &image, int width, int height, double threshold);
std::vector<double> nonMaximalSuppression(const std::vector<double> &image, int width, int height, int blockSize);
std::vector<Point> goodFeaturesToTrack(const std::vector<double> &image, int width, int height, double qualityLevel, double minimumDistance);
std::vector<uint8_t> convertImageTo8bit(const std::vector<double> &image, int width, int height, int channels, double gamma=2.2f);
std::vector<Point> lucasKanadeOpticalFlow(const std::vector<double> &prev, const std::vector<double> &next, int width, int height, const std::vector<Point> &features);