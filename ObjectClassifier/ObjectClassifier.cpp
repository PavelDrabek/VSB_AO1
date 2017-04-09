// ObjectClassifier.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#define METHOD_ETHALON 0
#define METHOD_KMEANS 1
#define METHOD_NEURAL 2

int main()
{
	// Choose method
	const int method = METHOD_ETHALON;
	Classifier classifier;

	// Helper matrices
	cv::Mat train_64fc1, index_8uc1, colorTrain_8uc3;
	cv::Mat test_64fc1, testIndex_8uc1, colorTest_8uc3, features_8uc3;

	cv::Mat train_8uc1 = cv::imread("../images/train.png", CV_LOAD_IMAGE_GRAYSCALE);
	// Load src and convert to double
	cv::Mat test_8uc1 = cv::imread("../images/test02.png", CV_LOAD_IMAGE_GRAYSCALE);

	// Convert to double
	train_8uc1.convertTo(train_64fc1, CV_64F, 1.0 / 255.0);
	test_8uc1.convertTo(test_64fc1, CV_64F, 1.0 / 255.0);

	// shapes & groups
	std::vector<shape> trainshapes;
	std::vector<shape> testshapes;
	std::vector<group> clusters;

	// Train shapes
	classifier.train(train_64fc1, index_8uc1, trainshapes);
	classifier.train(test_64fc1, testIndex_8uc1, testshapes);

	// Separate trained objects into groups
	if (method == METHOD_KMEANS) {
		classifier.generateClustersKMeans(clusters, trainshapes, 3);
	}
	else if (method == METHOD_ETHALON) {
		classifier.generateClusters(clusters, trainshapes);
	}

	// Color images
	classifier.colorIndexedObjects(index_8uc1, colorTrain_8uc3, trainshapes);
	classifier.colorIndexedObjects(testIndex_8uc1, colorTest_8uc3, testshapes);

	// Visualize features
	if (method == METHOD_ETHALON || method == METHOD_KMEANS) {
		classifier.visualizeFeatures(features_8uc3, trainshapes, clusters);
		cv::imshow("Features graph", features_8uc3);
	}

	// Apply recognition on test shapes
	if (method == METHOD_NEURAL) {
		classifier.neuralNetwork(trainshapes, testshapes);
	}
	else if (method == METHOD_ETHALON || method == METHOD_KMEANS) {
		classifier.recognize(clusters, testshapes);
	}

	// Annotate colored images
	classifier.annotate(colorTrain_8uc3, trainshapes);
	classifier.annotate(colorTest_8uc3, testshapes);

	// Show results
	cv::imshow("Train - Colored", colorTrain_8uc3);
	cv::imshow("Test - Colored", colorTest_8uc3);

	cv::waitKey(0);
	return 0;
}

