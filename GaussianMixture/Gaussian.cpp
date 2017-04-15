#include "stdafx.h"
#include "Gaussian.h"


Gaussian::Gaussian()
{
}


Gaussian::~Gaussian()
{
}

double Gaussian::calcP(double X)
{
	double e = std::exp(-((SQR(X - u) / (2.0 * SQR(sd)))));
	return (1.0 / (sd * std::sqrt(2.0 * M_PI))) * e;
}

void Gaussian::nextValue(double X, double sumP, double alpha, double minSD)
{
	// Probabilities
	double alphaP = alpha * (p * calcP(X)) / sumP;
	double newP = (1.0 - alpha) * p + alphaP;
	double ro = alphaP / newP;

	// SD and mean
	double newU = (1 - ro) * u + ro * X;
	double newSD = std::sqrt((1 - ro) * SQR(sd) + ro * SQR(X - newU));

	// Update new calculated values
	this->u = newU;
	this->sd = (newSD < minSD) ? minSD : newSD;
	this->p = newP;
}