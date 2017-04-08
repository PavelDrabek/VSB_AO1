// ObjectClassifier.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

int main()
{

	cv::Mat img = cv::imread("../images/train.png", CV_LOAD_IMAGE_COLOR);
	cv::Mat gray_img;
	cv::Mat gray_32f_img;

	// convert input color image to grayscale one, CV_BGR2GRAY specifies direction of conversion
	cv::cvtColor(img, gray_img, CV_BGR2GRAY);
	// convert grayscale image from 8 bits to 32 bits, resulting values will be in the interval 0.0 - 1.0
	gray_img.convertTo(gray_32f_img, CV_32FC1, 1.0 / 255.0);

	cv::imshow("test", gray_32f_img);
	cv::waitKey(0);

    return 0;
}

