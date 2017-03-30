#pragma once

#include <string>
#include <vector>
#include <ostream>

// Forward declaration
struct shape;

struct group {
public:
	int id;
	std::string name;
	double F1;
	double F2;
	std::vector<int> objIds;
	std::vector<shape*> objects;

	group(int id) : id(id) {}
	group(int id, double F1, double F2) : id(id), F1(F1), F2(F2) {}
	group(int id, std::string name, double F1, double F2) : id(id), name(name), F1(F1), F2(F2) {}
	~group();

	friend std::ostream &operator<<(std::ostream &os, const group &group);
};
