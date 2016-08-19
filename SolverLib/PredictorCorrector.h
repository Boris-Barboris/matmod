#pragma once

#include "InitialValueProblem.h"

// This structure is passed to each func
struct PC_ARG
{
	PC_ARG(bool log, bool *exit_flag) : log_flag(log), exit_flag(exit_flag) {}
	bool log_flag = false;		// set to true if accepting function must log something
	bool *exit_flag;			// set to true by func when corrector has achieved desired precision
	// funcs are advised to log when setting exit_flag to true
};

///////////////////////////////////////////////////////////////////////////////////
// Predictor-Corrector Cauchy problem solver, order of accuracy 2.
// Solves system of "n" y'(x) = f(x, y0, y1 ... yn)
// equations with initial values, represented by a pair of
// x = x0 number and y0 = (y1(x0), y2(x0) ... yn(x0)) vector.
// Predictor - explicit Euler's method (Runge-Kutta 1st) in boundary knots
// and Two-step Adams–Bashforth on main web.
// Corrector - trapesoidal rule
///////////////////////////////////////////////////////////////////////////////////
template <int n = 1> class PredictorCorrector : public InitialValueProblem<n>
{
public:
	PredictorCorrector(std::array<ODE_func, n> functions, double x0, n_doubles y0_arr) :
		InitialValueProblem(functions, x0, y0_arr) {}

	static const int MAX_ITER = 3;		// maximum amount of corrector iterations

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
		int solution_size = (x_max - x0) / dx + 1;

		// result matrix
		std::array<std::vector<double>, n + 1> solution;

		// initialize solution with x0
		for (int i = 0; i < n; i++)
		{
			solution[i].resize(solution_size);
			solution[i][0] = y0[i];
		}
		solution[n].resize(solution_size);
		solution[n][0] = x0;

		// cache f_n_last - array of derivatives from last knot,
		// f_n_last2 - from the knot before last
		bool nop;			// just placeholder
		n_doubles y_n = get_column(solution, 0);
		PC_ARG log_args(true, &nop);
		n_doubles f_n_last = diff(x, y_n, &log_args);
		n_doubles f_n_last2;

		// main iteration block
		for (int i = 0; i < solution_size - 1; i++)
		{
			y_n = get_column(solution, i);
			
			// Predictor
			n_doubles y_n1;
			if (i == 0)
				y_n1 = eulers(x, dx, y_n, f_n_last);
			else
				y_n1 = adams_bashforth2(dx, y_n, f_n_last2, f_n_last);
			//y_n1 = y_n;		// simple predictor

			// Corrector
			bool exit_flag = false;
			PC_ARG exit_args(false, &exit_flag);

			n_doubles f_n1;
			int k = 0;
			while (!exit_flag)
			{
				if (k >= MAX_ITER) break;
				if (k == MAX_ITER - 1)
					exit_args.log_flag = true;						// force log on MAX_ITER iteration exit
				f_n1 = diff(x + dx, y_n1, &exit_args);
				y_n1 = trapesium_rule(dx, y_n, f_n_last, f_n1);
				k++;
			}

			// Apply to solution
			f_n_last2 = f_n_last;
			f_n_last = f_n1;
			for (int j = 0; j < n; j++)
				solution[j][i + 1] = y_n1[j];
			x += dx;
			solution[n][i + 1] = x;
		}

		return solution;
	}

private:
	// get column from array of vectors
	inline n_doubles get_column(std::array<std::vector<double>, n + 1> &solution, int column)
	{
		n_doubles result;
		for (int i = 0; i < n; i++)
			result[i] = solution[i][column];
		return result;
	}

	// apply func-s in x on y with arg
	inline n_doubles diff(double x, n_doubles &y, PC_ARG *arg)
	{
		n_doubles result;
		for (int i = 0; i < n; i++)
			result[i] = func[i](x, y, arg);
		return result;
	}

	inline n_doubles eulers(double x, double h, n_doubles &y_n, n_doubles &f_n)
	{
		n_doubles result;
		for (int i = 0; i < n; i++)
			result[i] = y_n[i] + h * f_n[i];

		return result;
	}

	/*
	inline n_doubles runge_kutta2(double x, double h, n_doubles &y_n, n_doubles &f_n)
	{
		static bool nop;
		static PC_ARG log_args(false, &nop);

		n_doubles result;
		n_doubles temp;
		for (int i = 0; i < n; i++)
			temp[i] = y_n[i] + 0.5 * h * f_n[i];
		for (int i = 0; i < n; i++)
			result[i] = y_n[i] + h * func[i](x + h * 0.5, temp, &log_args);

		return result;
	}
	*/

	inline n_doubles adams_bashforth2(double h, n_doubles &y_n2, n_doubles &f_n1, n_doubles &f_n2)
	{
		n_doubles result;
		for (int i = 0; i < n; i++)
			result[i] = y_n2[i] + h * (1.5 * f_n2[i] - 0.5 * f_n1[i]);

		return result;
	}

	inline n_doubles trapesium_rule(double h, n_doubles &y_n, n_doubles &f_n, n_doubles &f_n1)
	{
		n_doubles result;
		for (int i = 0; i < n; i++)
			result[i] = y_n[i] + 0.5 * h * (f_n1[i] + f_n[i]);

		return result;
	}

};