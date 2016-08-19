#pragma once

#include "Stdafx.h"

///////////////////////////////////////////////////////////////////////////////////
// Tridiagonal matrix algorithm
// body - vector of triplets (A B C)
// http://en.wikipedia.org/wiki/Tridiagonal_matrix_algorithm
///////////////////////////////////////////////////////////////////////////////////
std::vector<double> matrixThomasSolve(std::vector<std::array<double, 3>> &body, std::vector<double> &F)
{
	int n;								// matrix size
	if ((n = body.size()) != F.size())
		throw std::runtime_error("matrixThomasSolve: vector parameters size mismatch.");

	std::vector<double> alpha(n);
	std::vector<double> beta(n);

	alpha[0] = body[0][2] / body[0][1];
	beta[0] = F[0] / body[0][1];

	// forward sweep

#define a body[i][0]
#define b body[i][1]
#define c body[i][2]
#define d F[i]

	for (int i = 1; i < n; i++)
	{
		alpha[i] = c / ( b - a * alpha[i-1] );
		beta[i] = ( d - a * beta[i-1] ) / ( b - a * alpha[i-1] );
	}

#undef a
#undef b
#undef c
#undef d

	// backward sweep

	std::vector<double> result(n);
	result[n-1] = beta[n-1];

	for (int i = n - 2; i >= 0; i--)
		result[i] = beta[i] - alpha[i] * result[i + 1];
	
	return result;
}