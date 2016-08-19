#pragma once

#include "Stdafx.h"

//#include "SolverLib.h"

///////////////////////////////////////////////////////////////////////////////////
// Unmanaged class, wich performs math library configuration and usage.
// Generally, it solves the modeling problem.
///////////////////////////////////////////////////////////////////////////////////
struct SolverClass
{
	SolverClass();
	
	double Ck, Lk, Rk, U0, I0, p0, T0, radius, length, dt, t_max;
	bool adapive;
	enum SolverAlgType { RK4, PK2 } alg;
	//SolverLib::SolverAlgorithm alg;

	// main function
	std::vector<std::vector<double>> calculateMacro();

	// calculates distribution by r of divergence of optical flux F
	// in 192 frequency intervals and summarizes it, returning divF(r)
	static std::vector<std::tuple<double, double>> calculateFluxApprox(int divisions, double p, double _I, double rad);

	// calculates distribution by r of divergence of optical flux F
	// in 192 frequency intervals and summarizes it, returning divF(r)
	static std::vector<std::tuple<double, double>> calculateFluxBalance(int divisions, double p, double _I, double rad);

	// modeling functions
private:
	//
	// Macroparameters section
	//
	double dIdt(double _t, std::array<double, 2> &args, void *log_flag_ptr);
	double dIdt_PC(double _t, std::array<double, 2> &args, void *pc_args, double *prev_r);
	double dUdt(double _t, std::array<double, 2> &args, void *log_flag_ptr);
	double dUdt_PC(double _t, std::array<double, 2> &args, void *pc_args, double *prev_U);
	double Rp_func(double _t, double _I, bool log_flag);
	double Rp_func_PC(double _t, double _I, void *pc_args, double *prev_r);
	double sigma_func(double z, double _I, double p);
	double p_func(double _I);
	double PZT(double p, double _I);
	
	static double T_fromI(double z, double _I);
	static double n_fromTp(double _T, double p);
	static double T_func(double z, double _T0, double m);

	//
	// Xenon microparameters section
	//
	static double u_nu_balance_func(double freq, double T);
	static double k_func(double freq, double p, double T);
	static double p_wave_func(double z);
	static double z_from_fi(double fi);
	
	// Set of static methods for differential approximation algorithm 
	struct BVP_approx
	{
		struct Parameters
		{
			double p, _I, rad, h;
			int divisions;
		};
		struct PrecalculatedValues
		{
			std::vector<double> *z, *r, *fi, *T, *p_wave_vector, *p_wave_dz_vector;
		};
		static std::vector<double> async_run(int start, int end, Parameters par, PrecalculatedValues pval);
		static double p_wave_dz_func(double z, double p_wave);
		static double B_func(double z, double k, double R, double h, double wave);
		static std::array<double, 2> AC_func(double z, double k, double k_dz, double R, double h, double wave, double wave_dz);
		static double D_func(double z, double k, double R, double u_nu_balance, double h);
	};

	// Set of static methods for balance method
	struct BVP_balance
	{
		struct PrecalculatedValues
		{
			std::vector<double> *z, *r, *T, *p_wave_vector;
		};
		static std::vector<double> async_run(int start, int end, BVP_approx::Parameters par, PrecalculatedValues pval);
		static double A_func(double z_0, double z_1, double z_2, double k_1);
		static double B_func(double A, double C, double R, double k, double V);
		static double C_func(double z_0, double z_1, double z_2, double k_1);
		static double D_func(double R, double k, double u_nu_balance, double V);
		static double K0_func(double R, double z_0, double z_2, double k_1, double p_wave_1, double h);
		static double M0_func(double R, double z_0, double z_2, double k_1, double p_wave_1, double h);
		static double P0_func(double h, double k_1, double p_wave_1, double u_nu_balance_1);
		static double Mn_func(double R, double z_0, double z_1, double z_2, double k_1, double k_2,
			double p_wave_1, double p_wave_2, double h);
		static double Kn_func(double R, double z_0, double z_1, double z_2, double k_1, double k_2,
			double p_wave_1, double p_wave_2, double h);
		static double Pn_func(double z_1, double k_1, double k_2, double p_wave_1, double p_wave_2, 
			double u_nu_balance_1, double u_nu_balance_2, double h);
	};

	// log interpolation optimization, prepared vectors and data here
private:
	//std::vector<double> T0_data;
	std::vector<double> sigma_t_axis_log;
	std::vector<std::vector<double>> sigma_data_log;

	// vectors for additional results
	std::vector<double> t2_result;
	std::vector<double> R_result;
	std::vector<double> p_result;
};