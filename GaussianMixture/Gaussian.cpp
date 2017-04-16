#include "stdafx.h"
#include "Gaussian.h"


Gaussian::Gaussian(double u, double sigma, double p)
{
	this->sigma = sigma;
	this->u = u;
	this->p = p;
}

Gaussian::~Gaussian()
{
}

double Gaussian::calcP(double X)
{
	double e = std::exp(-((SQR(X - u) / (2.0 * SQR(sigma)))));
	return (1.0 / (sigma * SQRT_2PI)) * e;

	//return 1.0f / (sd * SQRT_2PI) * std::exp(-(SQR(X - u) / (2 * SQR(sd))));
}

void Gaussian::nextValue(double X, double sumP, double alpha, double minSigma)
{
	// Probabilities
	double alphaP = alpha * ((p * calcP(X)) / sumP);
	double newP = (1.0 - alpha) * p + alphaP;
	double ro = alphaP / newP;

	// SD and mean
	double nU = (1 - ro) * u + ro * X;
	double nSigma = std::sqrt((1 - ro) * SQR(sigma) + ro * SQR(X - nU));

	// Update new calculated values
	this->u = nU;
	this->sigma = (nSigma < minSigma) ? minSigma : nSigma;
	this->p = newP;
}
