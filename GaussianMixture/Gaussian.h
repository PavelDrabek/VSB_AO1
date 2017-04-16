#pragma once
class Gaussian
{
public:
	Gaussian(double u, double sigma, double p);
	~Gaussian();

	double u;
	double sigma;
	double p;

	double calcP(double X);
	void nextValue(double X, double sumP, double alpha, double minSigma);
};

