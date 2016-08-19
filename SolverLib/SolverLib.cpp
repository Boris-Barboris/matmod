// This is the main DLL file.

//#include "Stdafx.h"
#include <vector>

#include "SolverLib.h"
#include "SolverClass.h"
#include "DataTables.h"

namespace SolverLib
{

	///////////////////////////////////////////////////////
	// Default values factory function
	///////////////////////////////////////////////////////

	CircuitModel ^CircuitModel::createDefaultModel()
	{
		return gcnew CircuitModel(
			60e-6,
			19e-6,
			25e-3,
			1500,
			0,
			0.36,
			300,
			0.35,
			12,
			1e-6,
			4e-4,
			false,
			SolverAlgorithm::RungeKutta4);
	}

	///////////////////////////////////////////////////////
	// main calculation function
	///////////////////////////////////////////////////////

	List<double> ^CircuitModel::convert(std::vector<double> &vect)
	{
		array<double>^ ret = gcnew array<double>(vect.size());
		if (vect.size())
		{
			pin_ptr<double> dest = &ret[0];
			std::memcpy(dest, vect.data(), vect.size() * sizeof(double));
		}
		return gcnew List<double>(ret);
	}

	void CircuitModel::_calculate()
	{
		SolverClass solver;

		solver.adapive = adaptive;
		solver.alg = (alg == SolverAlgorithm::RungeKutta4) ? SolverClass::SolverAlgType::RK4 : SolverClass::SolverAlgType::PK2;
		solver.Ck = Ck;
		solver.dt = dt;
		solver.I0 = I0;
		solver.length = length;
		solver.Lk = Lk;
		solver.p0 = p0;
		solver.radius = radius;
		solver.Rk = Rk;
		solver.T0 = T0;
		solver.t_max = t_max;
		solver.U0 = U0;

		try
		{
			auto result = solver.calculateMacro();
			Time = convert(result[2]);
			I = convert(result[0]);
			U = convert(result[1]);
			R = convert(result[3]);
			P = convert(result[4]);
			Time2 = convert(result[5]);
		}
		catch (std::exception &e)
		{
			mutex->ReleaseMutex();
			throw gcnew Exception(gcnew String(e.what()));
		}
	}

	///////////////////////////////////////////////////////
	// debug functions
	///////////////////////////////////////////////////////

	#include "Interpolation.h"
	#include "RootSearch.h"
	#include "NumericIntegration.h"

	Tuple<List<Double> ^, List<Double> ^> ^CircuitModel::plotTempareture(double _I, double _Tw)
	{
		List<Double> ^z_list = gcnew List<Double>();
		List<Double> ^T_list = gcnew List<Double>();

		double z = 0.0;
		double h = 1e-3;

		Axis_set Iaxis = Axis_set({ Axis_t(DataTables::T0mI::T0mI_axis(), InterpType::Interp) });
		auto m_data = DataTables::T0mI::T0mI_m();
		auto T0_data = DataTables::T0mI::T0mI_T0();

		double T_0 = Lerp<0>::interpolate(Iaxis, T0_data, { std::abs(_I) });	// interpolate
		double m = Lerp<0>::interpolate(Iaxis, m_data, { std::abs(_I) });		// interpolate

		for (; z <= 1.0 + h / 2.0; z += h)
		{
			z_list->Add(z);
			T_list->Add(T_0 + (_Tw - T_0) * std::pow(z, m));
		}

		return gcnew Tuple<List<Double> ^, List<Double> ^>(z_list, T_list);
	}

	// get T from I
	double T_fromI(double z, double _I)
	{
		static Axis_set Iaxis = Axis_set({ Axis_t(DataTables::T0mI::T0mI_axis(), InterpType::Interp) });
		static auto m_data = DataTables::T0mI::T0mI_m();
		static auto T0_data = DataTables::T0mI::T0mI_T0();

		double T_0 = Lerp<0>::interpolate(Iaxis, T0_data, { std::abs(_I) });	// interpolate
		double m = Lerp<0>::interpolate(Iaxis, m_data, { std::abs(_I) });		// interpolate

		double _T = T_0 + (2000.0 - T_0) * std::pow(z, m);						// find T
		return _T;
	}

	// finds n from nTp table
	double n_fromTp(double _T, double p)
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

	// balance function F(p, _I) = 0
	double PZT(double p, double _I)
	{
		static const int SIMPSON_DIVISIONS = 20;
		static const double p0 = 0.36;
		static const double T0 = 300;

		double const p_count = p0 * 7242.0 / T0;			// initial particle count
		double simp = integrate_Simpson([=](double z) { return z * n_fromTp(T_fromI(z, _I), p); },
			0, 1, SIMPSON_DIVISIONS);
		double f = p_count - 2 * simp;
		return f;
	}

	Tuple<List<Double> ^, List<Double> ^> ^CircuitModel::plotPressure(double imin, double imax)
	{
		List<Double> ^i_list = gcnew List<Double>();
		List<Double> ^p_list = gcnew List<Double>();

		double i = imin;
		double h = (imax - imin) / 100.0;

		for (; i <= imax + h / 2.0; i += h)
		{
			i_list->Add(i);
			p_list->Add(bisection(std::bind(PZT, std::placeholders::_1, i), 15, 1));
		}

		return gcnew Tuple<List<Double> ^, List<Double> ^>(i_list, p_list);
	}

	// sigma(T) = sigma(z, I, p)
	double sigma_func(double z, double _I, double p)
	{
		// get T
		double T = T_fromI(z, _I);

		// vectors initialization
		auto sigma_t_axis_log = DataTables::SigmaTp::SigmaTp_Taxis();
		std::for_each(sigma_t_axis_log.begin(), sigma_t_axis_log.end(), DataTables::logarithmize);

		auto sigma_data_log = DataTables::SigmaTp::SigmaTp_data();
		std::for_each(sigma_data_log.begin(), sigma_data_log.end(),
			[](std::vector<double> &v) {std::for_each(v.begin(), v.end(), DataTables::logarithmize); });

		// we will interpolate sigma from Sigma(T, p) table
		static auto axis = Axis_set(
		{
			Axis_t(sigma_t_axis_log, InterpType::Interp),
			Axis_t(DataTables::SigmaTp::SigmaTp_paxis(), InterpType::Interp)
		});
		//static auto sigma_data = DataTables::SigmaTp::SigmaTp_data();

		// planar interpolation
		double sigma = std::exp(Lerp<1>::interpolate(axis, sigma_data_log, { std::log(T), p }));

		return sigma > 0.0 ? sigma : 0.0;
	}

	double resistance_helper_func(double length, double radius, double i, double p)
	{
		return length /
			(2 * std::_Pi * radius * radius *
			integrate_Simpson([=](double z) { return z * sigma_func(z, i, p); },
			0, 1, 20));
	}

	Tuple<List<Double> ^, List<Double> ^> ^CircuitModel::plotResistance(double imin, double imax, double length, double radius)
	{
		List<Double> ^i_list = gcnew List<Double>();
		List<Double> ^r_list = gcnew List<Double>();

		double i = imin;
		double h = (imax - imin) / 100.0;

		for (; i <= imax + h / 2.0; i += h)
		{
			i_list->Add(i);
			double p = bisection(std::bind(PZT, std::placeholders::_1, i), 15, 1);
			double r = resistance_helper_func(length, radius, i, p);
			r_list->Add(r);
		}

		return gcnew Tuple<List<Double> ^, List<Double> ^>(i_list, r_list);
	}

	Tuple<List<Double> ^, List<Double> ^> ^CircuitModel::plotSpectre(double freqmin, double freqmax, double pressure, double temp)
	{
		List<Double> ^freq_list = gcnew List<Double>();
		List<Double> ^k_list = gcnew List<Double>();

		static auto axis = Axis_set(
		{
			Axis_t(DataTables::XeSpectrum::freq_interval_points, InterpType::ConstInterval),
			Axis_t(DataTables::XeSpectrum::temp_axis, InterpType::Interp),
			Axis_t(DataTables::XeSpectrum::p_axis, InterpType::Interp)
		});

		double f = freqmin;
		double h = (freqmax - freqmin) / 500.0;

		for (; f <= freqmax + h / 2.0; f += h)
		{
			freq_list->Add(f);
			double k = Lerp<2>::interpolate(axis, DataTables::XeSpectrum::k_data, { f, temp, pressure });
			k_list->Add(k);
		}

		return gcnew Tuple<List<Double> ^, List<Double> ^>(freq_list, k_list);
	}

	Tuple<List<Double> ^, List<Double> ^> ^CircuitModel::plotFlux(int divisions, double pressure, double _I, double rad, BVP_algorithm method)
	{
		try
		{
			std::vector<std::tuple<double, double>> pairs;
			if (method == BVP_algorithm::DiffApprox)
				pairs = SolverClass::calculateFluxApprox(divisions, pressure, _I, rad);
			else
				pairs = SolverClass::calculateFluxBalance(divisions, pressure, _I, rad);
			List<Double> ^r_list = gcnew List<Double>();
			List<Double> ^flux_list = gcnew List<Double>();

			for (auto i = pairs.begin(); i != pairs.end(); i++)
			{
				r_list->Add(std::get<0>(*i));
				flux_list->Add(std::get<1>(*i));
			}

			return gcnew Tuple<List<Double> ^, List<Double> ^>(r_list, flux_list);
		}
		catch (Exception ^e)
		{
			throw e;
		}		
	}

}


