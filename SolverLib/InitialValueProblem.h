#pragma once

#include "Stdafx.h"

///////////////////////////////////////////////////////////////////////////////////
// General template for abstract Cauchy problem solver
// solves system of "n" y'(x) = f(x, y0, y1 ... yn)
// equations with initial values, represented by a pair of
// x = x0 number and y0 = (y1(x0), y2(x0) ... yn(x0)) vector
///////////////////////////////////////////////////////////////////////////////////
template <int n = 1> class InitialValueProblem
{
public:
	// typedef for widely-used type
	typedef std::array<double, n> n_doubles;

	// in Ordinary Differential Equation y'(x) = f(x, y0, ... yn)
	// typedef for f
	// void * pointer is used for misc algorithm-speciic tasks
	typedef std::function<double (double, n_doubles &, void *)> ODE_func;

public:
	InitialValueProblem(std::array<ODE_func, n> functions, double x0, n_doubles y0_arr) :
		func(functions), x0(x0), y0(y0_arr) {}
	
	virtual ~InitialValueProblem() {}

	// main function, solves the problem and returns array of yi(x) approximation tables
	// last row is set to x values
	// step - initial step of integration
	// x_max - will approximate y(x) in interval [x0, x_max]
	// Different algorythm realizations must overload this function
	virtual std::array<std::vector<double>, n + 1> solve(double step, double x_max) = 0;

protected:
	double x0;								// initial x value
	n_doubles y0;							// array of initial values y(x0)
	std::array<ODE_func, n> func;			// array of functions f(x, y1...yn)
};