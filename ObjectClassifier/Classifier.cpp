#include "stdafx.h"
#include "classifier.h"

#define SQR(x) ((x) * (x))
#define RAD(x) ((x) * (M_PI / 180.0))
#define DEG(x) ((x) * (180.0 / M_PI))



cv::Vec3b classifier::randomColor() {
	return cv::Vec3b(rand() % 255, rand() % 255, rand() % 255);
}

void classifier::threshold(cv::Mat &src, double threshold) {
	for (int y = 0; y < src.rows; y++) {
		for (int x = 0; x < src.cols; x++) {
			double &px = src.at<double>(y, x);

			if (px > threshold) {
				px = 1.0;
			}
			else {
				px = 0.0;
			}
		}
	}
}

void classifier::indexObjects(cv::Mat &src, cv::Mat &dst, std::vector<Shape> &shapes) {
	// Index image
	dst = cv::Mat::zeros(src.size(), CV_8UC1);
	std::queue<cv::Point> q;
	int index = 1;

	for (int y = 0; y < src.rows; y++) {
		for (int x = 0; x < src.cols; x++) {
			// Found obj at x,y push to queue
			if (src.at<double>(y, x) > 0.0 && dst.at<uchar>(y, x) == 0) {
				q.push(cv::Point(x, y));

				// [Breadth first search]
				while (!q.empty()) {
					cv::Point p = q.front();
					q.pop();

					if (dst.at<uchar>(p) != 0) continue;

					// Get all 8 neighbours coordinates [S - south, N - north, W - west, E - east]
					cv::Point pW(p.x - 1, p.y), pNW(p.x - 1, p.y + 1), pN(p.x, p.y + 1), pNE(p.x + 1, p.y + 1);
					cv::Point pE(p.x + 1, p.y), pSE(p.x + 1, p.y - 1), pS(p.x, p.y - 1), pSW(p.x - 1, p.y - 1);

					// Check if pixels in neighbourhood are still object
					if (src.at<double>(pW) > 0.0 && dst.at<uchar>(pW) == 0) {
						q.push(pW);
					}
					if (src.at<double>(pNW) > 0.0 && dst.at<uchar>(pNW) == 0) {
						q.push(pNW);
					}
					if (src.at<double>(pN) > 0.0 && dst.at<uchar>(pN) == 0) {
						q.push(pN);
					}
					if (src.at<double>(pNE) > 0.0 && dst.at<uchar>(pNE) == 0) {
						q.push(pNE);
					}
					if (src.at<double>(pE) > 0.0 && dst.at<uchar>(pE) == 0) {
						q.push(pE);
					}
					if (src.at<double>(pSE) > 0.0 && dst.at<uchar>(pSE) == 0) {
						q.push(pSE);
					}
					if (src.at<double>(pS) > 0.0 && dst.at<uchar>(pS) == 0) {
						q.push(pS);
					}
					if (src.at<double>(pSW) > 0.0 && dst.at<uchar>(pSW) == 0) {
						q.push(pSW);
					}

					// Set "visited"
					dst.at<uchar>(p) = cv::saturate_cast<uchar>(index);
				}

				// Push new shape
				shapes.push_back(Shape(index, randomColor()));

				// Up index for other objects
				index++;
			}
		}
	}
}


double classifier::moment(int x, int y, int p, int q) {
	// In our case we don't need to multiply coordinates with image function
	// since it's almost the same
	return std::pow(x, p) * std::pow(y, q); // * f(x,y)
}

double classifier::momentNormalized(int x, int xt, int y, int yt, int p, int q) {
	// In our case we don't need to multiply coordinates with image function
	// since it's almost the same
	return std::pow((x - xt), p) * std::pow((y - yt), q); // * f(x,y)
}

void classifier::momentMinMax(Shape &s) {
	double b = 0.5 * (s.moments.u20 + s.moments.u02);
	double D = 0.5 * sqrt(4 * SQR(s.moments.u11) + SQR(s.moments.u20 - s.moments.u02));

	// Calc feature F2 = Umin / Umax
	s.features.F2 = (b - D) / (b + D);
}


void classifier::calcFeatures(cv::Mat &index, std::vector<Shape> &shapes) {
	// Calculate moments needed for center of mass
	for (int y = 0; y < index.rows; y++) {
		for (int x = 0; x < index.cols; x++) {
			uchar v = index.at<uchar>(y, x);

			if (v > 0) {
				shapes[v - 1].moments.m00 += moment(x, y, 0, 0);
				shapes[v - 1].moments.m01 += moment(x, y, 0, 1);
				shapes[v - 1].moments.m10 += moment(x, y, 1, 0);
			}
		}
	}

	// Calculate Center of mass, for other features we'll use normalized function
	for (Shape &s : shapes) {
		s.features.center = cv::Point(
			static_cast<int>(s.moments.m10 / s.moments.m00),
			static_cast<int>(s.moments.m01 / s.moments.m00)
		);
	}

	// Calculate perimeter and other normalized moments
	for (int y = 1; y < index.rows - 1; y++) {
		for (int x = 1; x < index.cols - 1; x++) {
			uchar v = index.at<uchar>(y, x);

			// Skip px where there's no object
			if (v == 0) continue;

			// Calculate u00, u20 and u02 moments
			Shape &s = shapes[v - 1];
			s.moments.u00 += momentNormalized(x, s.features.center.x, y, s.features.center.y, 0, 0);
			s.moments.u11 += momentNormalized(x, s.features.center.x, y, s.features.center.y, 1, 1);
			s.moments.u20 += momentNormalized(x, s.features.center.x, y, s.features.center.y, 2, 0);
			s.moments.u02 += momentNormalized(x, s.features.center.x, y, s.features.center.y, 0, 2);

			// Neighbourhood points
			cv::Point N(x, y - 1), S(x, y + 1), E(x + 1, y), W(x - 1, y);

			// Get pixels
			uchar vN = index.at<uchar>(N);
			uchar vS = index.at<uchar>(S);
			uchar vE = index.at<uchar>(E);
			uchar vW = index.at<uchar>(W);

			// If center pixel is not the same as at least one of its neighbours -> perimeter
			if (v != vN || v != vS || v != vW || v != vE) {
				s.features.perimeter++;
			}
		}
	}

	// Calculate F1, F2 features
	for (Shape &s : shapes) {
		s.features.area = s.moments.m00;
		s.features.F1 = SQR(s.features.perimeter) / (100 * s.features.area);
		momentMinMax(s);
	}
}

void classifier::train(cv::Mat &src, cv::Mat &index, std::vector<Shape> &shapes) {
	// Separate images from background
	threshold(src);

	// Index and extract shapes
	indexObjects(src, index, shapes);

	// Learn features for each shape
	calcFeatures(index, shapes);
}

void classifier::recognize(std::vector<Group> &trainGroups, std::vector<Shape> &testShapes) {
	// minDistance is bigger by default so it's overriden by actual distance when looping
	double minDistance = 1;

	// Calculate distance
	for (Shape &s1 : testShapes) {
		for (Group &s2 : trainGroups) {
			// Calc distance
			double distance = sqrt(SQR(s2.F1 - s1.features.F1) + SQR(s2.F2 - s1.features.F2));

			// If distance is smaller then previous, save train shape ID to test shape
			if (distance < minDistance) {
				minDistance = distance;
				s1.name = s2.name;
				s1.group = &s2;
			}
		}

		// Reset distance
		minDistance = 1;
	}
}

void classifier::colorIndexedObjects(cv::Mat &src, cv::Mat &dst, std::vector<Shape> &shapes) {
	// Fill destination
	dst = cv::Mat::zeros(src.size(), CV_8UC3);

	// Fill objects
	for (int y = 0; y < src.rows; y++) {
		for (int x = 0; x < src.cols; x++) {
			uchar index = src.at<uchar>(y, x);

			if (index > 0) {
				dst.at<cv::Vec3b>(y, x) = shapes[index - 1].color;
			}
		}
	}
}

void classifier::visualizeFeatures(cv::Mat &src, std::vector<Shape> &shapes, std::vector<Group> &groups, int size) {
	src = cv::Mat::zeros(size, size, CV_8UC3);

	// Draw grid
	for (int y = 0; y < src.rows; y += size / 10) {
		for (int x = 0; x < src.cols; x += size / 10) {
			cv::line(src, cv::Point(0, y), cv::Point(size - 1, y), cv::Scalar(40, 40, 40));
			cv::line(src, cv::Point(x, 0), cv::Point(x, size - 1), cv::Scalar(40, 40, 40));
		}
	}

	for (auto &&g : groups) {
		// Skip empty groups
		if (g.objects.size() <= 0) continue;

		cv::Point center(
			static_cast<int>(g.F1 * size),
			static_cast<int>(g.F2 * size)
		);

		// Draw group feature circle
		cv::circle(src, center, 4, cv::Vec3b(255, 255, 255), -1);

		// Draw group ids
		cv::putText(src, std::to_string(g.id), cv::Point(center.x + 10, center.y),
			CV_FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 2);
	}

	// Draw feature circles
	for (auto &&s : shapes) {
		cv::circle(src, cv::Point(
			static_cast<int>(s.features.F1 * size),
			static_cast<int>(s.features.F2 * size)
		), 2, s.color, -1);
	}
}

void classifier::annotate(cv::Mat &src, std::vector<Shape> shapes) {
	for (auto &&s : shapes) {
		std::string name = (s.name.length() <= 0) ? std::to_string(s.id) : s.name;
		name = (s.group != nullptr && s.name.length() <= 0) ? std::to_string(s.group->id) : name;
		cv::putText(src, name, s.features.center, CV_FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 2);
	}
}

void classifier::generateClusters(std::vector<Group> &groups, std::vector<Shape> &shapes) {
	double F1 = 0, F2 = 0;
	int groupCount = 3, groupItemsCount = 4;
	std::string names[3] = { "Square", "Star", "Rectangle" };

	for (int i = 0; i < groupCount; i++) {
		// Create group, assign id
		Group g(i);

		for (int j = 0; j < groupItemsCount; j++) {
			Shape &s = shapes[(i * groupItemsCount) + j];
			F1 += s.features.F1;
			F2 += s.features.F2;

			// Assign objects to group
			g.objects.push_back(&s);
			g.objIds.push_back(s.id);
		}

		g.name = names[i];
		g.F1 = F1 / groupItemsCount;
		g.F2 = F2 / groupItemsCount;
		groups.push_back(g);

		// Reset features
		F1 = F2 = 0;
	}

	// Assign group ID to shape
	for (Group &group : groups) {
		for (Shape *object : group.objects) {
			(*object).group = &group;
			(*object).name = names[group.id];
		}
	}
}

int classifier::recalculateCenter(std::vector<Group> &centroids, std::vector<Shape> &shapes) {
	int changed = 0;

	for (auto &centroid : centroids) {
		double F1 = 0, F2 = 0;

		// Skip empty centroids
		if (centroid.objects.size() <= 0) continue;

		for (const Shape *obj : centroid.objects) {
			F1 += (*obj).features.F1;
			F2 += (*obj).features.F2;
		}

		// Calculate new center and check if it changed
		double newF1 = F1 / static_cast<double>(centroid.objects.size());
		double newF2 = F2 / static_cast<double>(centroid.objects.size());

		if (newF1 != centroid.F1 && newF2 != centroid.F2) {
			centroid.F1 = newF1;
			centroid.F2 = newF2;
			changed++;
		}
	}

	return changed;
}

void classifier::generateClustersKMeans(std::vector<Group> &centroids, std::vector<Shape> &shapes, int k) {
	// Generate random centroids
	std::mt19937 rng(time(NULL));
	std::uniform_real_distribution<double> distribution(0.1, 0.9);
	for (int i = 0; i < k; i++) {
		centroids.push_back(Group(i, distribution(rng), distribution(rng)));
	}

	cv::Mat features;
	int changed = 1;

	while (changed != 0) {
		// Reset changed count for each new iteration
		changed = 0;

		// Clear shapes list assigned to centroids
		for (Group &centroid : centroids) {
			centroid.objects.clear();
		}

		// Assign each shape to its nearest centroid
		for (Shape &shape : shapes) {
			double minDistance = 1;
			Group *minCentroid = nullptr;

			// Calc distance between shape and each centroid, pick the smallest distance centroid
			for (Group &centroid : centroids) {
				double distance = sqrt(SQR(centroid.F1 - shape.features.F1) + SQR(centroid.F2 - shape.features.F2));
				if (distance < minDistance) {
					minDistance = distance;
					minCentroid = &centroid;
				}
			}

			// Assign nearest centroid
			if (minCentroid != nullptr) {
				// Check if centroid has changed
				if (shape.group != nullptr && shape.group->id != (*minCentroid).id) {
					changed++;
				}

				shape.group = minCentroid;
				(*minCentroid).objects.push_back(&shape);
			}
		}

		// Visualize features
		//classifier::visualizeFeatures(features, shapes, centroids);
		//cv::imshow("Features graph", features);
		//cv::waitKey(0);

		// Recalculate center of centroids
		changed += recalculateCenter(centroids, shapes);
	}
}

void classifier::neuralNetwork(std::vector<Shape> &trainShapes, std::vector<Shape> &testShapes) {
	std::string names[3] = { "Square", "Star", "Rectangle" };
	int classes[12] = { 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2 };
	cv::Mat trainData((int)trainShapes.size(), 2, CV_32FC1);
	cv::Mat trainClasses(trainData.rows, 3, CV_32FC1);

	// Fill train matrix;
	for (int y = 0; y < trainData.rows; y++) {
		trainData.at<float>(y, 0) = static_cast<float>(trainShapes[y].features.F1);
		trainData.at<float>(y, 1) = static_cast<float>(trainShapes[y].features.F2);

		trainClasses.at<float>(y, 0) = 0;
		trainClasses.at<float>(y, 1) = 0;
		trainClasses.at<float>(y, 2) = 0;
		trainClasses.at<float>(y, classes[y]) = 1;
	}

	// Set layers
	cv::Mat layers = cv::Mat(3, 1, CV_32S);
	layers.at<int>(0, 0) = 2;
	layers.at<int>(1, 0) = 5;
	layers.at<int>(2, 0) = 3;

	// Init neural network
	cv::Ptr<cv::ml::ANN_MLP> machineBrain = cv::ml::ANN_MLP::create();
	machineBrain->setLayerSizes(layers);
	machineBrain->setActivationFunction(cv::ml::ANN_MLP::SIGMOID_SYM);
	machineBrain->train(trainData, cv::ml::ROW_SAMPLE, trainClasses);

	// Load Test images
	cv::Mat testData((int)testShapes.size(), 2, CV_32FC1);
	for (int y = 0; y < testData.rows; y++) {
		testData.at<float>(y, 0) = static_cast<float>(testShapes[y].features.F1);
		testData.at<float>(y, 1) = static_cast<float>(testShapes[y].features.F2);
	}

	// Test images
	cv::Mat machineOutput;
	machineBrain->predict(testData, machineOutput);

	// Fill matched indexes
	for (int i = 0; i < testShapes.size(); i++) {
		testShapes[i].name = names[maxIndex<float>(
			machineOutput.at<float>(i, 0),
			machineOutput.at<float>(i, 1),
			machineOutput.at<float>(i, 2)
			)];
	}
}