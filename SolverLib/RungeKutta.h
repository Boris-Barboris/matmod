#pragma once

#include "InitialValueProblem.h"

///////////////////////////////////////////////////////////////////////////////////
// Runge-Kutta Cauchy problem solver, order of accuracy 4.
// Solves system of "n" y'(x) = f(x, y0, y1 ... yn)
// equations with initial values, represented by a pair of
// x = x0 number and y0 = (y1(x0), y2(x0) ... yn(x0)) vector
///////////////////////////////////////////////////////////////////////////////////
template <int n = 1>
class RungeKutta4: public InitialValueProblem<n>
{
public:
	RungeKutta4(std::array<ODE_func, n> functions, double x0, n_doubles y0_arr) :
		InitialValueProblem(functions, x0, y0_arr) {}
	
	// main function, solves the problem and returns array of yi(x) approximation tables
	// last row is set to x values
	// step - initial step of integration
	// x_max - will approximate y(x) in interval [x0, x_max]
	virtual std::array<std::vector<double>, n + 1> solve(double step, double x_max)
	{
		// operational x
		double x = x0;

		// operational x stepping
		double dx = ((x_max - x0) > 0) ? std::abs(step) : -std::abs(step);

		// width of result matrix
		int solution_size = (x_max - x0) / dx;

		// result matrix
		std::array<std::vector<double>, n + 1> solution;

		// first column initialization and vectors capacity definition
		for (int i = 0; i < n; i++)
		{
			solution[i].resize(solution_size + 1);
			solution[i][0] = y0[i];
		}
		solution[n].resize(solution_size + 1);
		solution[n][0] = x0;

		n_doubles yn;				// array for currently operated column

		// main iteration block
		for (int i = 0; i < solution_size; i++)
		{
			for (int k = 0; k < n; k++)			// yn = y(x)
				yn[k] = solution[k][i];

			yn = main_step(x, dx, yn);			// yn = y(x + dx)

			for (int k = 0; k < n; k++)			// solution y(x + dx) = yn
				solution[k][i + 1] = yn[k];

			x += dx;
			solution[n][i + 1] = x;
		}

		return solution;
	}

private:

	bool true_bool = true;
	bool false_bool = false;

	// k1 = f(xn, yn)
	inline n_doubles k1(double x, n_doubles &yn)
	{
		n_doubles result;
		for (int i = 0; i < n; i++)
			result[i] = func[i](x, yn, &true_bool);
		return result;
	}

	// k2 = f(xn + 1/2 * h, yn + 1/2 * h * k1)
	inline n_doubles k2(double x, double h, n_doubles &yn, n_doubles &k1_vector)
	{
		n_doubles result;
		n_doubles temp;
		for (int i = 0; i < n; i++)
			temp[i] = yn[i] + 0.5 * h * k1_vector[i];
		for (int i = 0; i < n; i++)
			result[i] = func[i](x + 0.5 * h, temp, &false_bool);
		return result;
	}

	// k3 = f(xn + 1/2 * h, yn + 1/2 * h * k2)
	inline n_doubles k3(double x, double h, n_doubles &yn, n_doubles &k2_vector)
	{
		n_doubles result;
		n_doubles temp;
		for (int i = 0; i < n; i++)
			temp[i] = yn[i] + 0.5 * h * k2_vector[i];
		for (int i = 0; i < n; i++)
			result[i] = func[i](x + 0.5 * h, temp, &false_bool);
		return result;
	}

	// k4 = f(xn + h, yn + h * k3)
	inline n_doubles k4(double x, double h, n_doubles &yn, n_doubles &k3_vector)
	{
		n_doubles result;
		n_doubles temp;
		for (int i = 0; i < n; i++)
			temp[i] = yn[i] + h * k3_vector[i];
		for (int i = 0; i < n; i++)
			result[i] = func[i](x + h, temp, &false_bool);
		return result;
	}

	// y(xn+1) = yn + 1/6 * h * (k1 + k2 + k3 + k4)
	n_doubles main_step(double x, double h, n_doubles &yn)
	{
		n_doubles k1_vector = k1(x, yn);
		n_doubles k2_vector = k2(x, h, yn, k1_vector);
		n_doubles k3_vector = k3(x, h, yn, k2_vector);
		n_doubles k4_vector = k4(x, h, yn, k3_vector);

		n_doubles step_result;
		for (int i = 0; i < n; i++)
			step_result[i] = yn[i] + h * (k1_vector[i] +
				2 * k2_vector[i] + 2 * k3_vector[i] + k4_vector[i]) / 6.0;

		return step_result;
	}
	
};