//#include "shape.h"
//#include "group.h"
#include "stdafx.h"

std::ostream &operator<<(std::ostream &os, const Shape &shape) {
	os << "Shape: " << shape.id << " name: " << shape.name << std::endl
		<< "Color: " << shape.color << std::endl
		<< "GroupID: " << ((shape.group != nullptr) ? std::to_string(shape.group->id) : "undefined") << std::endl
		<< "Moments: " << std::endl
		<< "  m00: " << shape.moments.m00 << std::endl
		<< "  m01: " << shape.moments.m01 << std::endl
		<< "  m10: " << shape.moments.m10 << std::endl
		<< "  u00: " << shape.moments.u00 << std::endl
		<< "  u11: " << shape.moments.u11 << std::endl
		<< "  u02: " << shape.moments.u02 << std::endl
		<< "  u20: " << shape.moments.u20 << std::endl
		<< "Features: " << std::endl
		<< "  area: " << shape.features.area << std::endl
		<< "  center: " << shape.features.center << std::endl
		<< "  perimeter: " << shape.features.perimeter << std::endl
		<< "  F1: " << shape.features.F1 << std::endl
		<< "  F2: " << shape.features.F2 << std::endl;

	return os;
}

bool Shape::operator==(const Shape &shape) const {
	return id == shape.id &&
		moments.m00 == shape.moments.m00 &&
		moments.m01 == shape.moments.m01 &&
		moments.m10 == shape.moments.m10;
}

bool Shape::operator!=(const Shape &shape) const {
	return !(shape == *this);
}

Shape::~Shape() {
	group = nullptr;
}