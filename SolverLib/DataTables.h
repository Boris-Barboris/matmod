#pragma once

///////////////////////////////////////////////////////////////////////////////////
// Set of tables, adapted for in-programm usage.
// Functions return those tables in format, used by Interpolation.h
///////////////////////////////////////////////////////////////////////////////////

#include "Stdafx.h"

namespace DataTables
{

	void logarithmize(double &x);
	void delogarithmize(double &x);

	//////////////////////////////////////////////////////////////
	// Singular koefficients
	//////////////////////////////////////////////////////////////
	extern const double Tw;								// used in simple T (gas temperature) calculations
	extern const double k_bolc;

	//////////////////////////////////////////////////////////////
	// SigmaTp table section
	//////////////////////////////////////////////////////////////
	namespace SigmaTp
	{
		#define SigmaTp_T_size 19
		#define SigmaTp_p_size 3

		extern double SigmaTp[SigmaTp_T_size + 1][SigmaTp_p_size + 1];

		std::vector<double> SigmaTp_Taxis();
		std::vector<double> SigmaTp_paxis();
		std::vector<std::vector<double>> SigmaTp_data();
	}

	/////////////////////////////////////////////////////////////
	// nTp table section
	/////////////////////////////////////////////////////////////
	namespace nTp
	{
		#define nTp_T_size 19
		#define nTp_p_size 3

		extern double nTp[nTp_T_size + 1][nTp_p_size + 1];

		std::vector<double> nTp_Taxis();
		std::vector<double> nTp_paxis();
		std::vector<std::vector<double>> nTp_data();	
	}

	/////////////////////////////////////////////////////////////
	// T0mI table section
	/////////////////////////////////////////////////////////////
	namespace T0mI
	{
		#define T0mI_I_size 9

		extern double T0mI[T0mI_I_size][3];

		std::vector<double> T0mI_axis();
		std::vector<double> T0mI_T0();
		std::vector<double> T0mI_m();
	}

	/////////////////////////////////////////////////////////////
	// Xenon spectre section
	/////////////////////////////////////////////////////////////
	namespace XeSpectrum
	{
		extern const int freq_interval_count;			// количество интервалов спектра
		extern const int temp_count;					// количество температур в таблице
		extern const int press_count;					// количество давлений в таблице

		extern std::vector<double> freq_interval_points;
		extern std::vector<double> temp_axis;
		extern std::vector<double> p_axis;
		extern std::vector<std::vector<std::vector<double>>> k_data;
	}

}