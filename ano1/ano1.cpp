// ano1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#define SQR(x) ((x)*(x))
#define RAD(x) ((x) * (M_PI / 180.0))
#define DEG(x) ((x) * (180.0 / M_PI))

void Tutorial() 
{
	cv::Mat img = cv::imread("../images/lena.png", CV_LOAD_IMAGE_COLOR); // load color image from file system to Mat variable, this will be loaded using 8 bits (uchar)
																	  //cv::imshow( "LENA", img );
	cv::Mat gray_img; // declare variable to hold grayscale version of img variable, gray levels wil be represented using 8 bits (uchar)
	cv::Mat gray_32f_img; // declare variable to hold grayscale version of img variable, gray levels wil be represented using 32 bits (float)

	cv::cvtColor(img, gray_img, CV_BGR2GRAY); // convert input color image to grayscale one, CV_BGR2GRAY specifies direction of conversion
	gray_img.convertTo(gray_32f_img, CV_32FC1, 1.0 / 255.0); // convert grayscale image from 8 bits to 32 bits, resulting values will be in the interval 0.0 - 1.0

	int x = 10, y = 15; // pixel coordinates

	uchar p1 = gray_img.at<uchar>(y, x); // read grayscale value of a pixel, image represented using 8 bits
	float p2 = gray_32f_img.at<float>(y, x); // read grayscale value of a pixel, image represented using 32 bits
	cv::Vec3b p3 = img.at<cv::Vec3b>(y, x); // read color value of a pixel, image represented using 8 bits per color channel

											// print values of pixels
	printf("p1 = %d\n", p1);
	printf("p2 = %f\n", p2);
	printf("p3[ 0 ] = %d, p3[ 1 ] = %d, p3[ 2 ] = %d\n", p3[0], p3[1], p3[2]);

	gray_img.at<uchar>(y, x) = 0; // set pixel value to 0 (black)

								  // draw a rectangle
	cv::rectangle(gray_img, cv::Point(65, 84), cv::Point(75, 94),
		cv::Scalar(50), CV_FILLED);

	// declare variable to hold gradient image with dimensions: width= 256 pixels, height= 50 pixels.
	// Gray levels wil be represented using 8 bits (uchar)
	cv::Mat gradient_8uc1_img(50, 256, CV_8UC1);

	// For every pixel in image, assign a brightness value according to the x coordinate.
	// This wil create a horizontal gradient.
	for (int y = 0; y < gradient_8uc1_img.rows; y++) {
		for (int x = 0; x < gradient_8uc1_img.cols; x++) {
			gradient_8uc1_img.at<uchar>(y, x) = x;
		}
	}

	// diplay images
	cv::imshow("Gradient", gradient_8uc1_img);
	cv::imshow("Lena gray", gray_img);
	cv::imshow("Lena gray 32f", gray_32f_img);

	cv::waitKey(0); // wait until keypressed
}

cv::Mat LoadGrayscaleImg(std::string path, int _type)
{
	cv::Mat img = cv::imread(path, CV_LOAD_IMAGE_COLOR);
	cv::Mat gray_img;
	cv::Mat gray_32f_img;

	// convert input color image to grayscale one, CV_BGR2GRAY specifies direction of conversion
	cv::cvtColor(img, gray_img, CV_BGR2GRAY);
	// convert grayscale image from 8 bits to 32 bits, resulting values will be in the interval 0.0 - 1.0
	gray_img.convertTo(gray_32f_img, _type, 1.0 / 255.0);

	return gray_32f_img;
}

template <typename T>
float Convolution(cv::Mat img, int y, int x, cv::Mat mask) 
{
	float value = 0;
	for (int i = 0; i < mask.rows; i++) {
		for (int j = 0; j < mask.cols; j++) {
			int aty = y + i;
			int atx = x + j;
			if (aty < 0 || aty >= img.rows || atx < 0 || atx >= img.cols) {
				continue;
			}
			value += img.at<T>(aty, atx) * mask.at<float>(i, j);
		}
	}

	return value;
}

template <typename T>
cv::Mat Gauss(cv::Mat source, int _type)
{
	cv::Mat gauss(source.rows, source.cols, _type);
	cv::Mat maskGaus = cv::Mat(3, 3, CV_32FC1, new float[9]{ 1, 2, 1, 2, 4, 2, 1, 2, 1 });
	float maskSum = 1.0f / 16.0f;
	for (int y = 0; y < source.rows; y++) {
		for (int x = 0; x < source.cols; x++) {
			float f = Convolution<T>(source, y, x, maskGaus) * maskSum;
			gauss.at<T>(y, x) = f;
		}
	}

	return gauss;
}

void Sobel(cv::String path) 
{
	cv::Mat gray_32f_img = LoadGrayscaleImg(path, CV_32FC1);
	int cols = gray_32f_img.cols;
	int rows = gray_32f_img.rows;

	cv::Mat maskFx = cv::Mat(3, 3, CV_32FC1, new float[9]{ -1, 0, 1, -2, 0, 2, -1, 0, 1 });
	cv::Mat maskFy = cv::Mat(3, 3, CV_32FC1, new float[9]{ -1, -2, -1, 0, 0, 0, 1, 2, 1 });

	cv::Mat gray_32f_edges(rows, cols, CV_32FC1);

	for (int y = 0; y < rows; y++)
	{
		for (int x = 0; x < cols; x++)
		{
			float fx = Convolution<float>(gray_32f_img, y, x, maskFx) / 9;
			float fy = Convolution<float>(gray_32f_img, y, x, maskFy) / 9;
			float e = sqrt(fx * fx + fy * fy);
			gray_32f_edges.at<float>(y, x) = e;
		}
	}

	cv::imshow("Original", gray_32f_img);
	cv::imshow("Sobel", gray_32f_edges);
	cv::waitKey(0); // wait until keypressed
}

cv::Mat Laplace(cv::String path)
{
	cv::Mat gray_32f_img = LoadGrayscaleImg(path, CV_32FC1);
	//cv::imshow("Original", gray_32f_img);

	int cols = gray_32f_img.cols;
	int rows = gray_32f_img.rows;
	cv::Mat maskLaplace = cv::Mat(3, 3, CV_32FC1, new float[9]{ 0, 1, 0, 1, -4, 1, 0, 1, 0 });

	cv::Mat gray_32f_edges(rows, cols, CV_32FC1);
	cv::Mat color_32f3_edges(rows, cols, CV_32FC3);

	gray_32f_img = Gauss<float>(gray_32f_img, CV_32FC1);
	//cv::imshow("Gauss", gray_32f_img);

	for (int y = 0; y < rows; y++)
	{
		for (int x = 0; x < cols; x++)
		{
			float f = Convolution<float>(gray_32f_img, y, x, maskLaplace);
			float e = sqrt(f * f);
			gray_32f_edges.at<float>(y, x) = e;
			if (f > 0) {
				color_32f3_edges.at<cv::Vec3f>(y, x) = cv::Vec3f(0, 1, 0);
			} else if (f < 0) {
				color_32f3_edges.at<cv::Vec3f>(y, x) = cv::Vec3f(0, 0, 1);
			}
		}
	}

	cv::imshow("Laplace Edges", gray_32f_edges);
	cv::imshow("Laplace colored", color_32f3_edges);
	cv::waitKey(0);

	return gray_32f_edges;
}

float LinearInterpolation(float c1, float c2, double t) {
	return c1 + ((c2 - c1) * t);
}

int K1 = 1.0, K2 = 10.0;
int bili = 0;
cv::Mat img_edge, img_edgeSimplif;

double ctan(double i) {
	if (DEG(i) == 90) return 0; // i == 90deg
	return 1.0 / std::tan(i);
}

void edge_simplif(cv::Mat& src, cv::Mat& dst, double t1 = 1.0, double t2 = 1.0)
{
	dst = cv::Mat::zeros(src.size(), CV_64FC1);

	// Helpers
	cv::Mat d_64fc1 = cv::Mat::zeros(src.size(), CV_64FC1);
	cv::Mat edge_64fc1 = cv::Mat::zeros(src.size(), CV_64FC1);
	cv::Mat ori_64fc1 = cv::Mat::zeros(src.size(), CV_64FC1);

	// Derivation
	for (int y = 1; y < src.rows - 1; y++) {
		for (int x = 1; x < src.cols - 1; x++) {
			// Central
			double c_nx = (src.at<double>(y, x - 1) - src.at<double>(y, x + 1)) / 2.0;
			double c_ny = (src.at<double>(y - 1, x) - src.at<double>(y + 1, x)) / 2.0;

			ori_64fc1.at<double>(y, x) = cv::fastAtan2((float)c_ny, (float)c_nx);
			d_64fc1.at<double>(y, x) = sqrt(SQR(c_nx) + SQR(c_ny));
		}
	}

	// Non-maximum suppression
	double Ep = 0.0, En = 0.0, E, alpha, oriDeg, oriRad;

	for (int y = 1; y < d_64fc1.rows - 1; y++) {
		for (int x = 1; x < d_64fc1.cols - 1; x++) {
			E = d_64fc1.at<double>(y, x);
			oriDeg = (int)ori_64fc1.at<double>(y, x) % 180;
			oriRad = RAD(oriDeg);

			// Check for oriDeg in one of 4 octans
			if (oriDeg >= 0 && oriDeg < 45) {
				alpha = std::tan(oriRad);
				Ep = alpha * d_64fc1.at<double>(y + 1, x - 1) + (1 - alpha) * d_64fc1.at<double>(y, x - 1);
				En = alpha * d_64fc1.at<double>(y - 1, x + 1) + (1 - alpha) * d_64fc1.at<double>(y, x + 1);
			}
			else if (oriDeg >= 45 && oriDeg < 90) {
				alpha = ctan(oriRad);
				Ep = alpha * d_64fc1.at<double>(y + 1, x - 1) + (1 - alpha) * d_64fc1.at<double>(y + 1, x);
				En = alpha * d_64fc1.at<double>(y - 1, x + 1) + (1 - alpha) * d_64fc1.at<double>(y - 1, x);
			}
			else if (oriDeg >= 90 && oriDeg < 135) {
				alpha = ctan(oriRad);
				Ep = alpha * d_64fc1.at<double>(y + 1, x + 1) + (1 - alpha) * d_64fc1.at<double>(y + 1, x);
				En = alpha * d_64fc1.at<double>(y - 1, x - 1) + (1 - alpha) * d_64fc1.at<double>(y - 1, x);
			}
			else if (oriDeg >= 135 && oriDeg < 180) {
				alpha = std::tan(oriRad);
				Ep = alpha * d_64fc1.at<double>(y + 1, x + 1) + (1 - alpha) * d_64fc1.at<double>(y, x + 1);
				En = alpha * d_64fc1.at<double>(y - 1, x - 1) + (1 - alpha) * d_64fc1.at<double>(y, x - 1);
			}

			// Check if E is maximum
			if (E > Ep && E > En) {
				edge_64fc1.at<double>(y, x) = E;
			}
		}
	}

	// Hysteresis
	cv::Point lookfor[]{
		cv::Point(1, 1),  cv::Point(1, 0),  cv::Point(1, -1),
		cv::Point(0, 1),					cv::Point(0, -1),
		cv::Point(-1, 1), cv::Point(-1, 0), cv::Point(-1, 1) };
	int lookForSize = 8;
	std::vector<cv::Point> q;

	for (int y = 0; y < edge_64fc1.rows; y++) {
		for (int x = 0; x < edge_64fc1.cols; x++) {
			if (edge_64fc1.at<double>(y, x) > t2) {
				q.push_back(cv::Point(x, y));

				while (!q.empty()) {
					cv::Point p = q[q.size() - 1];
					q.erase(q.begin());

					// Already visited
					if (dst.at<double>(p) > 0.0) {
						continue;
					}

					for (int i = 0; i < lookForSize; i++)
					{
						cv::Point pCheck = p + lookfor[i];
						if (edge_64fc1.at<double>(pCheck) >= t1 && edge_64fc1.at<double>(pCheck) <= t2 &&
							dst.at<double>(pCheck) <= 0.0) {
							q.push_back(pCheck);
						}
					}

					// Set "visited"
					dst.at<double>(p) = 1.0;
				}
			}
		}
	}
}

void on_change(int id)
{
	edge_simplif(img_edge, img_edgeSimplif, (1+K1) / 100.0, (1+K2) / 100.0);
	cv::imshow("Edge simplification", img_edgeSimplif);
}

int runEdgeSimplig(std::string path)
{
	img_edge = LoadGrayscaleImg(path, CV_64F);
	//img_edge.convertTo(img_edge, CV_64F, 1.0);
	img_edge.copyTo(img_edgeSimplif);

	cv::imshow("Original", img_edge);

	//img_edgeSimplif = cvCreateImage(cvGetSize(img_edge), img_edge->depth, img_edge->nChannels);
	edge_simplif(img_edge, img_edgeSimplif, K1 / 100.0, K2 / 100.0);

	cv::imshow("Edge simplification", img_edgeSimplif);

	cvNamedWindow("Edge simplification");
	cvCreateTrackbar("t1", "Edge simplification", &K1, 99, on_change);
	cvCreateTrackbar("t2", "Edge simplification", &K2, 99, on_change);

	cvWaitKey(0);

	return 0;
}

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

void MarkSameObject(cv::Mat& binaryMat, cv::Mat& mat, int sy, int sx, int index)
{
	cv::Vec2i lookfor[] { cv::Vec2i(-1, 0), cv::Vec2i(1, 0), cv::Vec2i(0, -1), cv::Vec2i(0, 1) };
	int lookForSize = 4;
	std::vector<cv::Vec2i> coords;
	coords.push_back(cv::Vec2i(sy, sx));

	while (coords.size() > 0) {
		for (int i = 0; i < lookForSize; i++)
		{
			int y = coords[0][0] + lookfor[i][0];
			int x = coords[0][1] + lookfor[i][1];

			if (x < 0 || x >= mat.cols || y < 0 || y >= mat.rows) {
				continue;
			}

			if (binaryMat.at<float>(y, x) == 1 && mat.at<char>(y, x) != index) {
				mat.at<char>(y, x) = index;
				coords.push_back(cv::Vec2i(y, x));
			}
		}
		coords.erase(coords.begin());
	}
}

cv::Mat IndexObject(cv::Mat binaryMat) {
	cv::Mat indexMat(binaryMat.rows, binaryMat.cols, CV_8UC1);
	int objectID = 2;
	for (int y = 0; y < binaryMat.rows; y++) {
		for (int x = 0; x < binaryMat.cols; x++) {
			float c = binaryMat.at<float>(y, x);
			if (c == 1) {
				MarkSameObject(binaryMat, indexMat, y, x, objectID++);
			}
		}
	}
	return indexMat;
}

void thresholdImage(std::string path)
{
	cv::Mat grayscaleMat = LoadGrayscaleImg(path, CV_32FC1);
	cv::Mat binaryMat;
	cv::threshold(grayscaleMat, binaryMat, 0, 1, cv::THRESH_BINARY);

	int max = 0;

	cv::Mat indexed = IndexObject(binaryMat);
	for (int y = 0; y < indexed.rows; y++) {
		for (int x = 0; x < indexed.cols; x++) {
			char c = indexed.at<char>(y, x);
			if (c > max) {
				max = c;
			}
			indexed.at<char>(y, x) = 20 * c;
		}
	}

	cv::imshow("indexed", indexed);
	cv::imshow("a", binaryMat);
	cv::waitKey(0);
}

int main(int argc, char* argv[])
{
	Sobel("../images/valve.png");
	Laplace("../images/valve.png");
	runEdgeSimplig("../images/valve.png");
	
	return 0;
}

