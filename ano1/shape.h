#pragma once

#include <opencv2/opencv.hpp>
#include <ostream>

struct group;

struct shape {
public:
	int id;
	cv::Vec3b color;

	// Shape name (usually after match)
	std::string name;
	group *group = nullptr;

	// Moments
	union {
		struct {
			double m00;
			double m01;
			double m10;

			double u00;
			double u11;
			double u20;
			double u02;
		};

		double data[7];
	} moments;

	// Features
	struct {
		cv::Point center;
		double perimeter;
		double area;
		double F1;
		double F2;
	} features;

	// Constructor
	shape(int id, cv::Vec3b color) : id(id), color(color) {}
	~shape();

	// Operators
	friend std::ostream &operator<<(std::ostream &os, const shape &shape);
	bool operator==(const shape &shape) const;
	bool operator!=(const shape &shape) const;
};