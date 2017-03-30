#include "stdafx.h"
#include "group.h"

std::ostream &operator<<(std::ostream &os, const group &group) {
	os << "name: " << group.name << " F1: " << group.F1 << " F2: " << group.F2 << " objSize: " << group.objects.size() << " objIds: ";

	for (auto &&id : group.objIds) {
		os << id << ", ";
	}

	os << std::endl;

	return os;
}

group::~group() {
	objects.clear();
}