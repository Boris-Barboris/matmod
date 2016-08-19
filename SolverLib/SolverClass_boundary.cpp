#include "Stdafx.h"

#include "SolverClass.h"

#include "Interpolation.h"
#include "DataTables.h"
#include "Thomas.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <future>

///////////////////////////////////////////////////////////////////////////////////
// Lab03 section, Boundary problem, differential approximation algorithm
///////////////////////////////////////////////////////////////////////////////////

static const double a = M_PI_2 - 0.18;					// fi(z) function curve is defined by this 0.07 value
static const double a_tan = tan(a);						// cache it's tangent
static const double M_NU = 0.786;						// approximates right boundary condition
static const double speed_of_light = 3e10;				// c
static const unsigned THREAD_COUNT = 4;					// change for another PC, if needed

// calculates distribution by r of divergence of optical flux F
// in 192 frequency intervals and summarizes it, returning divF(r)
std::vector<std::tuple<double, double>> SolverClass::calculateFluxApprox(int divisions, double p, double _I, double rad)
{
	// First we will shift from r to z and to fi
	// z is changed from 0 to 1, just like fi. Let's precache fi and r
	std::vector<double> z(divisions + 1);
	std::vector<double> r(divisions + 1);
	std::vector<double> fi(divisions + 1);

	// we can then cache T
	std::vector<double> T(divisions + 1);

	// result
	std::vector<std::tuple<double, double>> result_flux(divisions + 1);

	double h = 1.0 / (double)divisions;
	for (int i = 0; i <= divisions; i++)
	{
		fi[i] = i * h;
		z[i] = z_from_fi(fi[i]);
		r[i] = z[i] * rad;
		std::get<0>(result_flux[i]) = r[i];
		//std::get<1>(result_flux[i]) = fi[i];
		T[i] = T_fromI(z[i], _I);
	}
	//
	// diffirential approximation method
	//
	std::vector<double> p_wave_vector(divisions + 1);				// cache p_wave and it's derivative
	std::vector<double> p_wave_dz_vector(divisions + 1);
	for (int i = 0; i <= divisions; i++)
	{
		p_wave_vector[i] = p_wave_func(z[i]);
		p_wave_dz_vector[i] = BVP_approx::p_wave_dz_func(z[i], p_wave_vector[i]);
	}	
	//
	// Parallell block begin
	//
	BVP_approx::Parameters pars = { p, _I, rad, h, divisions };											// pack parameters
	BVP_approx::PrecalculatedValues preval = { &z, &r, &fi, &T, &p_wave_vector, &p_wave_dz_vector };	// and precalculated vectors
	int freq_step = DataTables::XeSpectrum::freq_interval_count / THREAD_COUNT;							// not always correct
	std::vector<std::future<std::vector<double>>> results(THREAD_COUNT);
	for (int i = 0; i < THREAD_COUNT; i++)
	{
		results[i] = std::async(std::launch::async, BVP_approx::async_run, freq_step * i, freq_step * (i + 1) - 1, pars, preval);
	}
	for (int i = 0; i < THREAD_COUNT; i++)
	{
		auto res = results[i].get();
		for (int j = 0; j <= divisions; j++)
			std::get<1>(result_flux[j]) += res[j];
	}
	//
	// Parallell block end
	//
	return result_flux;
}

double ledge_diff_h2(double y0, double y1, double y2, double h)
{
	double result = (-3.0 * y0 + 4.0 * y1 - y2) / 2.0 / h;
	return result;
}

double redge_diff_h2(double y0, double y1, double y2, double h)
{
	double result = (3.0 * y2 - 4.0 * y1 + y0) / 2.0 / h;
	return result;
}

double middle_diff_h2(double y0, double y2, double h)
{
	double result = (y0 + y2) / 2.0 / h;
	return result;
}

std::vector<double> SolverClass::BVP_approx::async_run(int start, int end, Parameters par, PrecalculatedValues pval)
{
#define h par.h
#define rad par.rad
#define z (*pval.z)
#define T (*pval.T)
#define p_wave_vector (*pval.p_wave_vector)
#define p_wave_dz_vector (*pval.p_wave_dz_vector)

	std::vector<double> result(par.divisions + 1);
	// 192 cycles on frequencies
	for (int i = start; i <= end; i++)
	{
		std::vector<double> k_vector(par.divisions + 1);						// cache k and u_balance
		std::vector<double> k_dz_vector(par.divisions + 1);						
		std::vector<double> u_balance_vector(par.divisions + 1);
		for (int j = 0; j <= par.divisions; j++)
		{
			k_vector[j] = k_func(DataTables::XeSpectrum::freq_interval_points[i], par.p, T[j]);
			u_balance_vector[j] = u_nu_balance_func(DataTables::XeSpectrum::freq_interval_points[i], T[j]);
		}
		// calculate k derivative
		k_dz_vector[0] = ledge_diff_h2(k_vector[0], k_vector[1], k_vector[2], h);
		for (int j = 1; j < par.divisions; j++)
			k_dz_vector[j] = middle_diff_h2(k_vector[j-1], k_vector[j+1], h);
		k_dz_vector[par.divisions] = redge_diff_h2(k_vector[par.divisions-2], k_vector[par.divisions - 1], k_vector[par.divisions], h);

		std::vector<std::array<double, 3>> matrix_body(par.divisions + 1);
		std::vector<double> matrix_column(par.divisions + 1);					// F column

		// fill main part of the matrix without boundary conditions
		for (int j = 1; j < par.divisions; j++)
		{
			auto AC_pair = BVP_approx::AC_func(z[j], k_vector[j], k_dz_vector[j], rad, h, p_wave_vector[j], p_wave_dz_vector[j]);
			matrix_body[j][0] = AC_pair[0];
			matrix_body[j][1] = BVP_approx::B_func(z[j], k_vector[j], rad, h, p_wave_vector[j]);
			matrix_body[j][2] = AC_pair[1];
			matrix_column[j] = BVP_approx::D_func(z[j], k_vector[j], rad, u_balance_vector[j], h);
		}

		// left boundary conditions
		matrix_body[0][1] = -1.5 / h;
		matrix_body[0][2] = 2.0 / h;
		// eliminate [0][3] to 0
		double k1 = -0.5 / h / matrix_body[1][2];
		matrix_body[0][1] = matrix_body[0][1] - k1 * matrix_body[1][0];
		matrix_body[0][2] = matrix_body[0][2] - k1 * matrix_body[1][1];
		matrix_column[0] = -k1 * matrix_column[1];

		// right boundary conditions
		matrix_body[par.divisions][0] = 4.0 * p_wave_vector[par.divisions] / 3.0 / k_vector[par.divisions] / h;
		matrix_body[par.divisions][1] = -(p_wave_vector[par.divisions] / k_vector[par.divisions] / h + M_NU);
		// eliminate [divisions][-1] to 0
		double k2 = -(p_wave_vector[par.divisions] / 3.0 / k_vector[par.divisions] / h) / matrix_body[par.divisions - 1][0];
		matrix_body[par.divisions][0] = matrix_body[par.divisions][0] - k2 * matrix_body[par.divisions - 1][1];
		matrix_body[par.divisions][1] = matrix_body[par.divisions][1] - k2 * matrix_body[par.divisions - 1][2];
		matrix_column[par.divisions] = -k2 * matrix_column[par.divisions - 1];

		// Solve the system, get y vector
		std::vector<double> y_vector = matrixThomasSolve(matrix_body, matrix_column);

		// accumulate divF
		for (int j = 0; j <= par.divisions; j++)
			result[j] += speed_of_light * k_vector[j] * (u_balance_vector[j] - y_vector[j]);
			//result[j] += y_vector[j];
	}
	return result;
}

std::vector<double> SolverClass::BVP_balance::async_run(int start, int end, BVP_approx::Parameters par, PrecalculatedValues pval)
{
	std::vector<double> result(par.divisions + 1);
	// 192 cycles on frequencies
	for (int i = start; i <= end; i++)
	{
		std::vector<double> k_vector(par.divisions * 2 + 1);
		std::vector<double> u_balance_vector(par.divisions * 2 + 1);
		for (int j = 0; j <= par.divisions * 2; j++)
		{
			double freq_med = 0.5 * (DataTables::XeSpectrum::freq_interval_points[i] + DataTables::XeSpectrum::freq_interval_points[i+1]);
			k_vector[j] = k_func(freq_med, par.p, T[j]);
			u_balance_vector[j] = u_nu_balance_func(freq_med, T[j]);
		}

		std::vector<std::array<double, 3>> matrix_body(par.divisions + 1);
		std::vector<double> matrix_column(par.divisions + 1);					// F column

		// fill main part of the matrix without boundary conditions
		for (int j = 1; j < par.divisions; j++)
		{
			matrix_body[j][0] = BVP_balance::A_func(z[2*j-2], z[2*j-1], z[2*j], k_vector[2*j-1]);
			matrix_body[j][2] = BVP_balance::C_func(z[2*j], z[2*j+1], z[2*j+2], k_vector[2*j+1]);
			double V = (z[2*j+1] * z[2*j+1] - z[2*j-1] * z[2*j-1]) * 0.5;
			matrix_body[j][1] = BVP_balance::B_func(matrix_body[j][0], matrix_body[j][2], rad, k_vector[2*j], V);
			matrix_column[j] = BVP_balance::D_func(rad, k_vector[2*j], u_balance_vector[2*j], V);
		}

		// left boundary conditions
		matrix_body[0][1] = BVP_balance::K0_func(rad, z[0], z[2], k_vector[1], p_wave_vector[1], h);
		matrix_body[0][2] = BVP_balance::M0_func(rad, z[0], z[2], k_vector[1], p_wave_vector[1], h);
		matrix_column[0] = BVP_balance::P0_func(h, k_vector[1], p_wave_vector[1], u_balance_vector[1]);

		// right boundary conditions
		int end = 2 * par.divisions;
		matrix_body[par.divisions][0] = BVP_balance::Kn_func(rad, z[end-2], z[end-1], z[end], k_vector[end-1],
			k_vector[end], p_wave_vector[end-1], p_wave_vector[end], h);
		matrix_body[par.divisions][1] = BVP_balance::Mn_func(rad, z[end-2], z[end-1], z[end], k_vector[end-1],
			k_vector[end], p_wave_vector[end-1], p_wave_vector[end], h);
		matrix_column[par.divisions] = BVP_balance::Pn_func(z[end-1], k_vector[end-1], k_vector[end], p_wave_vector[end-1],
			p_wave_vector[end], u_balance_vector[end-1], u_balance_vector[end], h);

		// Solve the system, get y vector
		std::vector<double> y_vector = matrixThomasSolve(matrix_body, matrix_column);

		// accumulate divF
		for (int j = 0; j <= par.divisions; j++)
			result[j] += speed_of_light * k_vector[j * 2] * (u_balance_vector[j * 2] - y_vector[j]) * 
			(DataTables::XeSpectrum::freq_interval_points[i+1] - DataTables::XeSpectrum::freq_interval_points[i]);
			//result[j] += y_vector[j];
	}
	return result;

#undef h
#undef rad
#undef z
#undef T
#undef p_wave_vector
#undef p_wave_dz_vector
}

std::vector<std::tuple<double, double>> SolverClass::calculateFluxBalance(int divisions, double p, double _I, double rad)
{
	// First we will shift from r to z and to fi
	// z is changed from 0 to 1, just like fi. Let's precache fi and r
	std::vector<double> z(2 * divisions + 1);
	std::vector<double> r(2 * divisions + 1);

	// we can then cache T
	std::vector<double> T(2 * divisions + 1);

	// result
	std::vector<std::tuple<double, double>> result_flux(divisions + 1);

	double h = 0.5 / (double)divisions;
	for (int i = 0; i <= divisions * 2; i++)
	{
		double fi = i * h;
		z[i] = z_from_fi(fi);
		r[i] = z[i] * rad;
		T[i] = T_func(z[i], 1e4, 4.0);
		//T[i] = T_fromI(z[i], _I, 4.0);
		if (i % 2 == 0)
		{
			std::get<0>(result_flux[i / 2]) = r[i];
		}
	}

	//
	// Balance method
	//
	std::vector<double> p_wave_vector(divisions * 2 + 1);				// cache p_wave and it's derivative
	for (int j = 0; j <= divisions * 2; j++)
		p_wave_vector[j] = p_wave_func(z[j]);
	//
	// Parallell block begin
	//
	BVP_approx::Parameters pars = { p, _I, rad, 2 * h, divisions };							// pack parameters
	BVP_balance::PrecalculatedValues preval = { &z, &r, &T, &p_wave_vector};				// and precalculated vectors
	int freq_step = DataTables::XeSpectrum::freq_interval_count / THREAD_COUNT;				// not always correct
	std::vector<std::future<std::vector<double>>> results(THREAD_COUNT);
	for (int i = 0; i < THREAD_COUNT; i++)
	{
		results[i] = std::async(std::launch::async, BVP_balance::async_run, freq_step * i, freq_step * (i + 1) - 1, pars, preval);
	}
	for (int i = 0; i < THREAD_COUNT; i++)
	{
		auto res = results[i].get();
		for (int j = 0; j <= divisions; j++)
			std::get<1>(result_flux[j]) += res[j];
	}
	//
	// Parallell block end
	//
	return result_flux;
}

// Planck law function
double SolverClass::u_nu_balance_func(double freq, double T)
{
	return 6.1679e-4 * std::pow(freq, 3) / (double)(std::exp(47990.0 * freq / T) - 1.0);
}

static auto axis = Axis_set(
{
	Axis_t(DataTables::XeSpectrum::freq_interval_points, InterpType::ConstInterval),
	Axis_t(DataTables::XeSpectrum::temp_axis, InterpType::Interp),
	Axis_t(DataTables::XeSpectrum::p_axis, InterpType::Interp)
});

// coefficient of absorption on Xenon
double SolverClass::k_func(double freq, double p, double T)
{
	double k = Lerp<2>::interpolate(axis, DataTables::XeSpectrum::k_data, { freq, T, p });
	return k;
}

// p_waved function of coordinate switch
double SolverClass::p_wave_func(double z)
{
	double tan_sqr = tan(a * z);
	tan_sqr *= tan_sqr;
	double result = a / a_tan * (1.0 + tan_sqr);
	return result;
}

// axis transition
inline double SolverClass::z_from_fi(double fi)
{
	return atan(fi * a_tan) / a;
}

///////////////////////////////////////////////////////////////////////////////////
// differential approximation algorithm
///////////////////////////////////////////////////////////////////////////////////

// p_waved function, derived by z
inline double SolverClass::BVP_approx::p_wave_dz_func(double z, double p_wave)
{
	double _tan = tan(a * z);
	double cos_sqr = cos(a * z);
	cos_sqr *= cos_sqr;
	double result = 2.0 * a * a * _tan / a_tan / cos_sqr / p_wave;
	return result;
}

// Ax(n-1) + Bx(n) + Cx(n+1) = D - tridiagonal matrix row form

// In tridiagonal matrix it's coefficient before x(n)
inline std::array<double, 2> SolverClass::BVP_approx::AC_func(double z, double k, double k_dz, double R, double h, double wave, double wave_dz)
{
	double sum2 = 0.5 * wave / z;
	double sum3 = wave * wave / h;
	double sum4 = wave / k / 2.0 * (wave_dz * k - k_dz * wave);
	std::array<double, 2> result;
	result[0] = sum2 - sum3 + sum4;
	result[1] = -sum2 - sum3 - sum4;
	return result;
}

// In tridiagonal matrix it's coefficient before x(n)
inline double SolverClass::BVP_approx::B_func(double z, double k, double R, double h, double wave)
{
	double wave_sqr = wave * wave;
	double result = (2 * wave_sqr / h) + 3.0 * R * k * k * h;
	return result;
}

// In tridiagonal matrix it's right side value
inline double SolverClass::BVP_approx::D_func(double z, double k, double R, double u_nu_balance, double h)
{
	double result = 3.0 * R * k * k * u_nu_balance * h;
	return result;
}

///////////////////////////////////////////////////////////////////////////////////
// balance method
///////////////////////////////////////////////////////////////////////////////////

// In tridiagonal matrix it's coefficient before x(n-1)
inline double SolverClass::BVP_balance::A_func(double z_0, double z_1, double z_2, double k_1)
{
	double result = z_1 / k_1 / (z_2 - z_0);
	return result;
}

// In tridiagonal matrix it's coefficient before x(n)
inline double SolverClass::BVP_balance::B_func(double A, double C, double R, double k, double V)
{
	double result = A + C + 3.0 * R * R * k * V;
	return -result;
}

// In tridiagonal matrix it's coefficient before x(n+1)
inline double SolverClass::BVP_balance::C_func(double z_0, double z_1, double z_2, double k_1)
{
	double result = z_1 / k_1 / (z_2 - z_0);
	return result;
}

// In tridiagonal matrix it's right side value
inline double SolverClass::BVP_balance::D_func(double R, double k, double u_nu_balance, double V)
{
	double result = 3.0 * R * R * k * V * u_nu_balance;
	return -result;
}

//
// Boundary conditions
//

double SolverClass::BVP_balance::K0_func(double R, double z_0, double z_2, double k_1, double p_wave_1, double h)
{
	double result_left = 1.0 / 3.0 / R / R / k_1 / (z_2 - z_0);
	double result_right = h / 8.0 * k_1 / p_wave_1;
	double result = result_left + result_right;
	return result;
}

double SolverClass::BVP_balance::M0_func(double R, double z_0, double z_2, double k_1, double p_wave_1, double h)
{
	double result_left = h / 8.0 * k_1 / p_wave_1;
	double result_right = 1.0 / 3.0 / R / R / k_1 / (z_2 - z_0);
	double result = result_left - result_right;
	return result;
}

double SolverClass::BVP_balance::P0_func(double h, double k_1, double p_wave_1, double u_nu_balance_1)
{
	double result = 0.25 * k_1 * h * u_nu_balance_1 / p_wave_1;
	return result;
}

double SolverClass::BVP_balance::Mn_func(double R, double z_0, double z_1, double z_2, double k_1, double k_2,
	double p_wave_1, double p_wave_2, double h)
{
	double sum1 = M_NU * 0.5 / R;
	double sum2 = z_1 / 3.0 / R / R / k_1 / (z_2 - z_0);
	double sum3 = h * k_2 / 4.0 / p_wave_2;
	double sum4 = h / 8.0 * k_1 * z_1 / p_wave_1;
	double result = sum1 + sum2 + sum3 + sum4;
	return result;
}

double SolverClass::BVP_balance::Kn_func(double R, double z_0, double z_1, double z_2, double k_1, double k_2,
	double p_wave_1, double p_wave_2, double h)
{
	double left = h / 8.0 * k_1 * z_1 / p_wave_1;
	double right = z_1 / 3.0 / R / R / k_1 / (z_2 - z_0);
	double result = left - right;
	return result;
}

double SolverClass::BVP_balance::Pn_func(double z_1, double k_1, double k_2, double p_wave_1, double p_wave_2,
	double u_nu_balance_1, double u_nu_balance_2, double h)
{
	double left = k_2 * u_nu_balance_2 / p_wave_2;
	double right = k_1 * z_1 / p_wave_1 * u_nu_balance_1;
	double result = 0.25 * h * (left + right);
	return result;
}