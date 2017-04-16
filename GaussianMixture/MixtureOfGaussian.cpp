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

int MixtureOfGaussian::getIndex(int y, int x, int i)
{
	return y * cols * N + x * N + i;
}

MixtureOfGaussian::MixtureOfGaussian(cv::Mat mat, int N)
{
	this->N = N;
	this->cols = mat.cols;
	this->rows = mat.rows;

	printf("rows: %d, cols: %d, N: %d\n", rows, cols, N);

	data = (Gaussian*)malloc(rows * cols * N * sizeof(Gaussian));
	if (data == NULL) exit(1);

	int prevVal = -1;
	for (int y = 0; y < rows; y++) {
		for (int x = 0; x < cols; x++) {
			for (int i = 0; i < N; i++) {
				double u = i * (255 / N);
				double sigma = 255 / N;
				data[getIndex(y, x, i)] = Gaussian(u, sigma, 1.0 / N);
			}
		}
	}
}


MixtureOfGaussian::~MixtureOfGaussian()
{
}

double MixtureOfGaussian::sumP(double X, int y, int x)
{
	int pxIndex = getIndex(y, x, 0);
	double sum = 0;
	for (int i = 0; i < N; i++) {
		Gaussian &g = data[pxIndex + i];
		sum += g.p * g.calcP(X);
	}

	return sum;
}

void MixtureOfGaussian::nextFrame(cv::Mat &frame, cv::Mat &output) 
{
	for (int y = 0; y < frame.rows; y++) {
		for (int x = 0; x < frame.cols; x++) {
			int pxIndex = getIndex(y, x, 0);

			double px = (double)frame.at<cv::Vec3b>(y, x)[0];
			double sum = sumP(px, y, x);

			Gaussian maxg = data[pxIndex + 0];
			for (int i = 0; i < N; i++) {
				Gaussian* g = &data[pxIndex + i];
				g->nextValue(px, sum, 0.1, 4);
				if (g->p > maxg.p) {
					maxg = *g;
				}
			}

			double minPx = maxg.u - (2 * maxg.sigma);
			double maxPx = maxg.u + (2 * maxg.sigma);
			if (px > minPx && px < maxPx) {
				output.at<uchar>(y, x) = 0;
			} else {
				output.at<uchar>(y, x) = 255;
			}
		}
	}
}

void MixtureOfGaussian::visualize(cv::Mat &dst, cv::Point &center, int X) {
	// Init variables
	dst = cv::Mat::zeros(256, 256, CV_8UC3);
	std::vector<std::vector<cv::Point_<double>>> graphsPoints;
	double maxY = 0;

	int pxIndex = getIndex(center.y, center.x, 0);

	// Gaussian colors
	cv::Vec3b colors[5] = {
		cv::Vec3b(244, 67, 54),
		cv::Vec3b(103, 58, 183),
		cv::Vec3b(33, 150, 243),
		cv::Vec3b(76, 175, 80),
		cv::Vec3b(63, 81, 181),
	};

	// Find maxY and extract gaussian points
	for (int x = 0; x < dst.cols - 1; x++) {
		std::vector<cv::Point_<double>> graphsPoint;

		// Extract points for each gaussian
		for (int i = 0; i < N; i++) {
			double y = data[pxIndex + i].calcP(x);
			graphsPoint.push_back(cv::Point_<double>(x, y));

			// Find maxY
			if (y > maxY) {
				maxY = y;
			}
		}

		graphsPoints.push_back(graphsPoint);
	}

	double offset = 240 / maxY;

	// Draw gaussian points with given offset
	for (int i = 1; i < graphsPoints.size(); i++) {
		for (int j = 0; j < graphsPoints[i].size(); j++) {
			cv::Point p1(
				static_cast<int>(graphsPoints[i - 1][j].x),
				static_cast<int>(255 - offset * graphsPoints[i - 1][j].y)
			);
			cv::Point p2(
				static_cast<int>(graphsPoints[i][j].x),
				static_cast<int>(255 - offset * graphsPoints[i][j].y)
			);

			// Draw line between 2 points
			cv::line(dst, p1, p2, colors[j % 5], 1);
		}
	}

	// Draw line where the pixel value is located now
	cv::line(dst, cv::Point(X, 0), cv::Point(X, dst.rows), cv::Vec3b(0, 255, 0), 1);
	cv::putText(dst, std::to_string(X), cv::Point(X + 15, 10), CV_FONT_HERSHEY_SIMPLEX, 0.5, cv::Vec3b(255, 255, 255));

	for (int i = 0; i < N; i++) {
		Gaussian &g = data[pxIndex + i];
		std::stringstream stream;
		stream << "p: " << std::fixed << std::setprecision(2) << g.p;
		stream << " u: " << std::fixed << std::setprecision(2) << g.u;
		stream << " sigma: " << std::fixed << std::setprecision(2) << g.sigma;
		std::string s = stream.str();
		cv::putText(dst, stream.str(), cv::Point(5, 30 + i * 15), CV_FONT_HERSHEY_SIMPLEX, 0.5, cv::Vec3b(255, 255, 255));
	}

}

