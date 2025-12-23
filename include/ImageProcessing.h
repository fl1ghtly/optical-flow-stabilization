#pragma once
#include <cstdint>
#include <vector>
#include <algorithm>
#include <cmath>

struct Corner {
	float response;
	int x;
	int y;

	bool operator<(const Corner& rhs) const { return this->response < rhs.response; }
	bool operator>(const Corner& rhs) const { return this->response > rhs.response; }
	bool operator<=(const Corner& rhs) const { return this->response <= rhs.response; }
	bool operator>=(const Corner& rhs) const { return this->response >= rhs.response; }
};

std::vector<float> convolveImageKernel(const std::vector<float> &image, int width, int height, std::vector<std::vector<float>> kernel);
std::vector<float> boxFilter(const std::vector<float> &image, int width, int height, int boxSize, bool normalize=false);
std::vector<float> calculateCovarianceMatrix(const std::vector<float> &image, int width, int height, int blockSize);
std::vector<float> harrisCornerDetector(const std::vector<float> &image, int width, int height, int blockSize, float sensitivity);
std::vector<float> shiTomasiCornerDetector(const std::vector<float> &image, int width, int height, int blockSize);
std::vector<float> threshold(const std::vector<float> &image, int width, int height, float threshold);
std::vector<float> nonMaximalSuppression(const std::vector<float> &image, int width, int height, int blockSize);
std::vector<Corner> goodFeaturesToTrack(const std::vector<float> &image, int width, int height, float qualityLevel, float minimumDistance);
uint8_t* convertImageTo8bit(const std::vector<float> &image, int width, int height, int channels, float gamma=2.2f);