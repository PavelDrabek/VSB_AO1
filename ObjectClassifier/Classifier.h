#pragma once

#include "stdafx.h"

#define SQR(x) ((x) * (x))
#define RAD(x) ((x) * (M_PI / 180.0))
#define DEG(x) ((x) * (180.0 / M_PI))

class Classifier {
public:

	template<typename T>
	int maxIndex(T x, T y, T z);

	void threshold(cv::Mat &src, double t);

	cv::Vec3b randomColor();

	void indexObjects(cv::Mat &src, cv::Mat &dst, std::vector<shape> &shapes);

	double moment(int x, int y, int p, int q);

	double momentNormalized(int x, int xt, int y, int yt, int p, int q);

	void momentMinMax(shape &s);

	void calcFeatures(cv::Mat &index, std::vector<shape> &shapes);

	void train(cv::Mat &src, cv::Mat &index, std::vector<shape> &shapes);

	void recognize(std::vector<group> &traingroups, std::vector<shape> &testshapes);

	void colorIndexedObjects(cv::Mat &src, cv::Mat &dst, std::vector<shape> &shapes);

	void visualizeFeatures(cv::Mat &src, std::vector<shape> &shapes, std::vector<group> &groups, int size = 400);

	void annotate(cv::Mat &src, std::vector<shape> shapes);

	void generateClusters(std::vector<group> &groups, std::vector<shape> &shapes);

	int recalculateCenter(std::vector<group> &centroids, std::vector<shape> &shapes);

	void generateClustersKMeans(std::vector<group> &centroids, std::vector<shape> &shapes, int k);

	void neuralNetwork(std::vector<shape> &trainshapes, std::vector<shape> &testshapes);

};