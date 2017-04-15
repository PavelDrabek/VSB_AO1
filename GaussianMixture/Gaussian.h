#pragma once
class Gaussian
{
public:
	Gaussian();
	~Gaussian();

	double sd;
	double u;
	double p;

	double calcP(double X);
	void nextValue(double X, double sumP, double alpha, double minSD);
};

