#pragma once
#include "Gaussian.h"

class MixtureOfGaussian
{
public:
	MixtureOfGaussian(cv::Mat mat, int N);
	~MixtureOfGaussian();

	void nextFrame(cv::Mat& frame, cv::Mat& output);
	double sumP(double X, int y, int x);

private:
	cv::Mat_<Gaussian> gauss;
	int N;
};

