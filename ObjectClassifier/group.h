#ifndef VSB_ANO_GROUP_H
#define VSB_ANO_GROUP_H

#include "stdafx.h"

// Forward declaration
struct Shape;

struct Group {
public:
	int id;
	std::string name;
	double F1;
	double F2;
	std::vector<int> objIds;
	std::vector<Shape*> objects;

	Group(int id) : id(id) {}
	Group(int id, double F1, double F2) : id(id), F1(F1), F2(F2) {}
	Group(int id, std::string name, double F1, double F2) : id(id), name(name), F1(F1), F2(F2) {}
	~Group();

	friend std::ostream &operator<<(std::ostream &os, const Group &group);
};

#endif //VSB_ANO_GROUP_H
