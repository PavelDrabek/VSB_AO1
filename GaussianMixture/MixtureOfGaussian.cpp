#include "stdafx.h"
#include "MixtureOfGaussian.h"

std::string type2str(int type) {
	std::string r;

	uchar depth = type & CV_MAT_DEPTH_MASK;
	uchar chans = 1 + (type >> CV_CN_SHIFT);

	switch (depth) {
	case CV_8U:  r = "8U"; break;
	case CV_8S:  r = "8S"; break;
	case CV_16U: r = "16U"; break;
	case CV_16S: r = "16S"; break;
	case CV_32S: r = "32S"; break;
	case CV_32F: r = "32F"; break;
	case CV_64F: r = "64F"; break;
	default:     r = "User"; break;
	}

	r += "C";
	r += (chans + '0');

	return r;
}

MixtureOfGaussian::MixtureOfGaussian(cv::Mat mat, int N)
{
	this->N = N;
	const int sz[] = { mat.rows, mat.cols, N };
	gauss = cv::Mat_<Gaussian>(3, sz);

	for (int y = 0; y < mat.rows; y++) {
		for (int x = 0; x < mat.cols; x++) {
			for (int i = 0; i < N; i++) {
				gauss.at<Gaussian>(y, x, i) = Gaussian();
			}
		}
	}
}


MixtureOfGaussian::~MixtureOfGaussian()
{
}

double MixtureOfGaussian::sumP(double X, int y, int x)
{
	double sum = 0;
	for (int i = 0; i < N; i++) {
		sum += X * gauss.at<Gaussian>(y, x, i).calcP(X);
	}
	return sum;
}

void MixtureOfGaussian::nextFrame(cv::Mat &frame, cv::Mat &output) 
{
	//printf("color: %s\n", type2str(frame.type()).c_str());
	//printf("color: %f\n", (double)frame.at<cv::Vec3b>(100, 100)[0]);

	for (int y = 0; y < frame.rows; y++) {
		for (int x = 0; x < frame.cols; x++) {
			double px = (double)frame.at<cv::Vec3b>(y, x)[0];
			double sum = sumP(px, y, x);

			int maxGaussIndex = 0;
			for (int i = 0; i < N; i++) {
				gauss.at<Gaussian>(y, x, i).nextValue(px, sum, 0.1, 4);
				if (gauss.at<Gaussian>(y, x, i).p > gauss.at<Gaussian>(y, x, maxGaussIndex).p) {
					maxGaussIndex = i;
				}
			}

			// Detect changes
			Gaussian &g = gauss.at<Gaussian>(y, x, maxGaussIndex);
			double minPx = g.u - (g.sd * 2);
			double maxPx = g.u + (g.sd * 2);

			if (px > minPx && px < maxPx) {
				output.at<uchar>(y, x) = 0; // background
			} else {
				output.at<uchar>(y, x) = 255; // change
			}
		}
	}
}