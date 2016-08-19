#pragma once

///////////////////////////////////////////////////////////////////////////////////
// 1-dimensional numeric integration functions
///////////////////////////////////////////////////////////////////////////////////

#include "Stdafx.h"

double integrate_Simpson(std::function<double(double)> f, double a, double b, double h)
{
	double result = 0.0;

	for (double x = a; x <= b; x += h)
		result += h / 6.0 * (f(x) + 4 * f(x + h / 2.0) + f(x + h));

	return result;
}

double integrate_Simpson(std::function<double(double)> f, double a, double b, int steps)
{
	double dx = (b - a) / (double)steps;
	return integrate_Simpson(f, a, b, dx);
}