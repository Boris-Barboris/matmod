#pragma once

#include "Stdafx.h"

///////////////////////////////////////////////////////////////////////////////////
// Binary root search (bisection), returns x, wich evaluates f(x) to 0.0
// with initial root assumption x0 and "spread" to find other-sign interval.
// llimit - left limit for search interval
///////////////////////////////////////////////////////////////////////////////////
double bisection(const std::function<double(double)> f, double x0, double llimit, double spread = 1.0)
{
	static const int MAX_ITER = 1000;
	static const double EPS = 1e-3;

	if (f(x0) == 0.0)
		return x0;

	if (f(0.0) == 0.0)
		return 0.0;

	double x1 = (x0 > llimit) ? x0 : llimit;
	double x2 = x1 + spread;
	x2 = (x2 > llimit) ? x2 : llimit;
	int iter_count = 0;

	// search interval vith ends of diffirent sign
	double fx1 = f(x1);
	while (fx1 * f(x2) > 0.0)
	{
		if (iter_count++ > MAX_ITER)
			throw std::runtime_error("binary_search: can't find root!");
		spread = - (2 * spread);
		x2 = x1 + spread;
		x2 = (x2 > llimit) ? x2 : llimit;
	}

	// found interval
	if (x1 > x2) std::swap(x1, x2);
	double x = (x1 + x2) / 2.0;

	// bissection itself
	while ( std::abs((x2 - x1) / (x ? x : x + EPS)) >= EPS)
	{
		if (iter_count++ > MAX_ITER)
			throw std::runtime_error("binary_search: can't find root!");
		double f1 = f(x1);
		double f2 = f(x2);
		double fx = f(x);
		if ( f1 * fx < 0.0)
		{
			x2 = x;
			x = (x1 + x2) / 2.0;
		}
		else if ( f2 * fx < 0.0)
		{
			x1 = x;
			x = (x1 + x2) / 2.0;
		}
		else if (f1 == 0.0)
			return x1;
		else if (f2 == 0.0)
			return x2;
		else if (fx == 0.0)
			return x;
		else
			throw std::runtime_error("binary_search: can't find root!");
	}

	return x;
}