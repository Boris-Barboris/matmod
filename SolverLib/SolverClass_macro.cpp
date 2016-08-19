#include "Stdafx.h"

#include "SolverClass.h"

#include "RungeKutta.h"
#include "Interpolation.h"
#include "NumericIntegration.h"
#include "RootSearch.h"
#include "DataTables.h"
#include "PredictorCorrector.h"

using namespace std::placeholders;

///////////////////////////////////////////////////////////////////////////////////
// Lab02 section, Cauchy problem
///////////////////////////////////////////////////////////////////////////////////

SolverClass::SolverClass()
{
	// vectors initialization
	sigma_t_axis_log = DataTables::SigmaTp::SigmaTp_Taxis();
	std::for_each(sigma_t_axis_log.begin(), sigma_t_axis_log.end(), DataTables::logarithmize);

	sigma_data_log = DataTables::SigmaTp::SigmaTp_data();
	std::for_each(sigma_data_log.begin(), sigma_data_log.end(),
		[](std::vector<double> &v) {std::for_each(v.begin(), v.end(), DataTables::logarithmize);});
}

std::vector<std::vector<double>> SolverClass::calculateMacro()
{
	// Formalize initial value problem
	InitialValueProblem<2> *apparatus;
	decltype(apparatus->solve(1.0, 1.0)) solution;

	switch (alg)
	{
		case (RK4):
		{
			// create function array
			std::array<RungeKutta4<2>::ODE_func, 2> func_arr =
			{ 
				std::bind(&SolverClass::dIdt, this, _1, _2, _3),
				std::bind(&SolverClass::dUdt, this, _1, _2, _3)
			};
			// create initial value
			RungeKutta4<2>::n_doubles initial_values = { I0, U0 };
			// create solver object
			apparatus = new RungeKutta4<2>(func_arr, 0.0, initial_values);
			// First log
			//Rp_func(0.0, I0, false);
			break;
		}
		case (PK2) :
		{
			double prev_i, prev_p;
			// create function array
			std::array<PredictorCorrector<2>::ODE_func, 2> func_arr =
			{
				std::bind(&SolverClass::dIdt_PC, this, _1, _2, _3, &prev_i),
				std::bind(&SolverClass::dUdt_PC, this, _1, _2, _3, &prev_p)
			};
			// create initial value
			PredictorCorrector<2>::n_doubles initial_values = { I0, U0 };
			// create solver object
			apparatus = new PredictorCorrector<2>(func_arr, 0.0, initial_values);
			break;
		}
		default:
			return { {} };
	}

	solution = apparatus->solve(dt, t_max);

	std::vector<std::vector<double>> result(6);
	result[0] = std::move(solution[0]);
	result[1] = std::move(solution[1]);
	result[2] = std::move(solution[2]);
	result[3] = std::move(R_result);
	result[4] = std::move(p_result);
	result[5] = std::move(t2_result);

	return result;
}

#define _I args[0]
#define _U args[1]

// dI/dt = f1(t, I, U)
double SolverClass::dIdt(double _t, std::array<double, 2> &args, void *log_flag_ptr)
{
	return (_U - _I * (Rk + Rp_func(_t, _I, *static_cast<bool *>(log_flag_ptr)))) / Lk;
}

// dI/dt = f1(t, I, U)
double SolverClass::dIdt_PC(double _t, std::array<double, 2> &args, void *pc_args, double *prev_r)
{
	return (_U - _I * (Rk + Rp_func_PC(_t, _I, pc_args, prev_r))) / Lk;
}

// dU/dt = f2(t, I, U)
double SolverClass::dUdt(double _t, std::array<double, 2> &args, void *log_flag_ptr)
{
	return - _I / Ck;
}

double SolverClass::dUdt_PC(double _t, std::array<double, 2> &args, void *pc_args, double *prev_U)
{
	static const double EPS = 1e-3;

	double u = - _I / Ck;

	// Exit condition
	PC_ARG *pc_arg = static_cast<PC_ARG *>(pc_args);
	if (std::abs((u - *prev_U) / u) < EPS)
		*pc_arg->exit_flag = true;

	// save last state
	*prev_U = u;

	return u;
}

#undef _I
#undef _U

// Rp(I)
double SolverClass::Rp_func(double _t, double _I, bool log_flag)
{
	static const int SIMPSON_DIVISIONS = 20;

	double p = p_func(_I);

	double res = length /
		(2 * std::_Pi * radius * radius *
		integrate_Simpson([=](double z) { return z * sigma_func(z, _I, p); },
						  0, 1, SIMPSON_DIVISIONS));

	if (log_flag)
	{
		t2_result.push_back(_t);
		R_result.push_back(res);
		p_result.push_back(p);
	}
	
	return res;
}

double SolverClass::Rp_func_PC(double _t, double _I, void *pc_args, double *prev_r)
{
	static const int SIMPSON_DIVISIONS = 20;
	static const double EPS = 1e-3;

	PC_ARG *pc_arg = static_cast<PC_ARG *>(pc_args);

	double p = p_func(_I);

	double res = length /
		(2 * std::_Pi * radius * radius *
		integrate_Simpson([=](double z) { return z * sigma_func(z, _I, p); },
		0, 1, SIMPSON_DIVISIONS));

	// exit condition
	if (std::abs((res - *prev_r) / res) < EPS)
	{
		*pc_arg->exit_flag = true;
		pc_arg->log_flag = true;
	}

	if (pc_arg->log_flag)
	{
		t2_result.push_back(_t);
		R_result.push_back(res);
		p_result.push_back(p);
	}

	// save last state
	*prev_r = res;

	return res;
}

// sigma(T) = sigma(z, I, p)
double SolverClass::sigma_func(double z, double _I, double p)
{
	// get T
	double T = T_fromI(z, _I);

	/*
	// we will interpolate sigma from Sigma(T, p) table
	static auto axis = Axis_t({ DataTables::SigmaTp::SigmaTp_Taxis(), DataTables::SigmaTp::SigmaTp_paxis() });
	static auto sigma_data = DataTables::SigmaTp::SigmaTp_data();

	// planar interpolation
	double sigma = Lerp<1>::interpolate(axis, sigma_data, { T, p });
	*/

	static auto axis = Axis_set(
	{
		Axis_t(sigma_t_axis_log, InterpType::Interp),
		Axis_t(DataTables::SigmaTp::SigmaTp_paxis(), InterpType::Interp)
	});

	double sigma = std::exp(Lerp<1>::interpolate(axis, sigma_data_log, { std::log(T), p }));

	return sigma > 0.0 ? sigma : 0.0;
}

// get T from I
double SolverClass::T_fromI(double z, double _I)
{
	static Axis_set Iaxis = Axis_set({ Axis_t(DataTables::T0mI::T0mI_axis(), InterpType::Interp) });
	static auto m_data = DataTables::T0mI::T0mI_m();
	static auto T0_data = DataTables::T0mI::T0mI_T0();

	double T_0 = Lerp<0>::interpolate(Iaxis, T0_data, { std::abs(_I) });	// interpolate
	double m = Lerp<0>::interpolate(Iaxis, m_data, { std::abs(_I) });		// interpolate

	double _T = T_func(z, T_0, m);											// find T
	return _T;
}

// T(z, T0, m) = T0 + (Tw - T0) * pow(z, m)
double SolverClass::T_func(double z, double _T0, double m)
{
	return _T0 + (DataTables::Tw - _T0) * std::pow(z, m);
}

// finds p from I
double SolverClass::p_func(double _I)
{	
	double p = bisection(std::bind(&SolverClass::PZT, this, _1, _I), 15, 1);
	return p;
}

// balance function F(p, _I) = 0
double SolverClass::PZT(double p, double _I)
{
	static const int SIMPSON_DIVISIONS = 20;

	double const p_count = p0 * 7242.0 / T0;			// initial particle count
	double simp = integrate_Simpson([=](double z) { return z * n_fromTp(T_fromI(z, _I), p); }, 
									0, 1, SIMPSON_DIVISIONS);
	double f = p_count - 2 * simp;
	return f;
}

// finds n from nTp table
double SolverClass::n_fromTp(double _T, double p)
{
	// we will interpolate n from n(T, p) table
	static auto axis = Axis_set(
	{
		Axis_t(DataTables::nTp::nTp_Taxis(), InterpType::Interp),
		Axis_t(DataTables::nTp::nTp_paxis(), InterpType::Interp)
	});
	static auto sigma_data = DataTables::nTp::nTp_data();

	// planar interpolation
	double n = Lerp<1>::interpolate(axis, sigma_data, { _T, p });

	return n;
}