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
	return 1.0f / (sigma * SQRT_2PI) * std::exp(-(SQR(X - u) / (2 * SQR(sigma))));
}

void Gaussian::nextValue(double X, double sumP, double alpha, double minSigma)
{
	double alphaP = alpha * ((p * calcP(X)) / sumP);
	double nP = (1.0 - alpha) * p + alphaP;
	double ro = alphaP / nP;

	double nU = (1 - ro) * u + ro * X;
	double nSigma = std::sqrt((1 - ro) * SQR(sigma) + ro * SQR(X - nU));

	u = nU;
	sigma = (nSigma < minSigma) ? minSigma : nSigma;
	p = nP;
}
