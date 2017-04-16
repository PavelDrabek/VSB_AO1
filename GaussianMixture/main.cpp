// GaussianMixture.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "MixtureOfGaussian.h"

#include <ctime>

int frameCount = 0;
clock_t t1, t2 = clock();
void drawFPS(cv::Mat &mat)
{
	t1 = t2;
	t2 = clock();
	int fps = (int)(1000 / (double)(t2 - t1));

	cv::putText(mat, std::to_string(fps), cvPoint(20, 30),
		cv::FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(0, 0, 255), 1, CV_AA);
	cv::putText(mat, std::to_string(frameCount), cvPoint(20, 50),
		cv::FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(0, 0, 255), 1, CV_AA);

	frameCount++;
}

cv::Point point(400, 400);
void CallBackFunc(int event, int x, int y, int flags, void *userdata) {
	if (event == CV_EVENT_LBUTTONDOWN) {
		point.x = x;
		point.y = y;
	}
}

int main()
{
	const std::string filename= "../images/dt_passat.mpg";
	cv::VideoCapture video;
	video.open(filename); // load color image from file system to Mat variable, this will be loaded using 8 bits (uchar)

	if (!video.isOpened()) {
		return -1;	
	}
	
	cv::Mat frame, detection, graph;

	video >> frame;
	
	detection = cv::Mat(frame.rows, frame.cols, CV_8UC1);

	MixtureOfGaussian mog(frame, 3);
	while (true) {

		video >> frame;
		
		mog.nextFrame(frame, detection);

		//mog.visualize(graph, point, frame.at<cv::Vec3b>(point)[0]);
		//cv::imshow("graph", graph);

		drawFPS(frame);
		cv::setMouseCallback("frame", CallBackFunc, NULL);
		cv::imshow("frame", frame);
		cv::imshow("detection", detection);
		if (cv::waitKey(1) == ' ') {
			break;
		}
	}

	video.release();
	//cv::waitKey(0);

    return 0;
}

