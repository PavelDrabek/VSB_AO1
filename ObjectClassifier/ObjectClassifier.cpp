// ObjectClassifier.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#define METHOD_ETHALON 0
#define METHOD_KMEANS 1
#define METHOD_NEURAL 2

void classify(cv::Mat train_64fc1, cv::Mat test_64fc1, const int method)
{
	// Helper matrices
	cv::Mat colorTrain_8uc3, colorTest_8uc3, index_8uc1, testIndex_8uc1;

	// Shapes & groups
	std::vector<Shape> trainShapes;
	std::vector<Shape> testShapes;
	std::vector<Group> clusters;

	// Train shapes
	classifier::train(train_64fc1, index_8uc1, trainShapes);
	classifier::train(test_64fc1, testIndex_8uc1, testShapes);


	// Color images
	classifier::colorIndexedObjects(testIndex_8uc1, colorTest_8uc3, testShapes);
	classifier::colorIndexedObjects(index_8uc1, colorTrain_8uc3, trainShapes);

	// Visualize features
	if (method == METHOD_ETHALON || method == METHOD_KMEANS) {
		// Separate trained objects into groups
		if (method == METHOD_KMEANS) {
			classifier::generateClustersKMeans(clusters, trainShapes, 3);
		} else if (method == METHOD_ETHALON) {
			classifier::generateClusters(clusters, trainShapes);
		}

		classifier::recognize(clusters, testShapes);
	} else if (method == METHOD_NEURAL) {
		// Apply recognition on test shapes
		classifier::neuralNetwork(trainShapes, testShapes);
	}

	// Annotate colored images
	classifier::annotate(colorTest_8uc3, testShapes);
	classifier::annotate(colorTrain_8uc3, trainShapes);

	std::string methodName = "";
	switch (method)
	{
	case METHOD_ETHALON: methodName = "ethalon"; break;
	case METHOD_KMEANS: methodName = "kmeans"; break;
	case METHOD_NEURAL: methodName = "neuron"; break;
	default:
		break;
	}

	// Show results
	cv::imshow(methodName, colorTest_8uc3);
	cv::imshow("Train objects", colorTrain_8uc3);
}

int main() {
	// Helper matrices
	cv::Mat train_64fc1, index_8uc1, colorTrain_8uc3;
	cv::Mat test_64fc1, testIndex_8uc1;

	cv::Mat train_8uc1 = cv::imread("../images/train.png", CV_LOAD_IMAGE_GRAYSCALE);
	// Load src and convert to double
	cv::Mat test_8uc1 = cv::imread("../images/test02.png", CV_LOAD_IMAGE_GRAYSCALE);

	// Convert to double
	train_8uc1.convertTo(train_64fc1, CV_64F, 1.0 / 255.0);
	test_8uc1.convertTo(test_64fc1, CV_64F, 1.0 / 255.0);


	classify(train_64fc1, test_64fc1, METHOD_ETHALON);
	classify(train_64fc1, test_64fc1, METHOD_KMEANS);
	classify(train_64fc1, test_64fc1, METHOD_NEURAL);
	cv::waitKey(0);

	return 0;
}

