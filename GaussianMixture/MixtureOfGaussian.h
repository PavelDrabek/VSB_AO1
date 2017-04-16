#pragma once
#include "Gaussian.h"

class MixtureOfGaussian
{
public:
	MixtureOfGaussian(cv::Mat mat, int N);
	~MixtureOfGaussian();

	void nextFrame(cv::Mat& frame, cv::Mat& output);
	double sumP(double X, int y, int x);
	void visualize(cv::Mat &dst, cv::Point &center, int x);

private:
	std::vector<std::vector<std::vector<Gaussian>>> gauss;
	int N;
};

