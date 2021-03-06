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
	Gaussian* data;

	int getIndex(int y, int x, int i);
	int rows, cols;
	int N;
};

