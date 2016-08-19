// SolverLib.h

///////////////////////////////////////////////////////////////////////////////////
// main .NET library file
// provides CLR interface for model class CircuitModel
///////////////////////////////////////////////////////////////////////////////////

#pragma once

//#include "Stdafx.h"

using namespace System;
using namespace System::Collections::Generic;
using namespace System::Drawing;
using namespace System::ComponentModel;
using namespace System::ComponentModel::Design;
using namespace System::Reflection;

namespace SolverLib 
{

	public enum class SolverAlgorithm
	{
		RungeKutta4,
		PredictorCorrector
	};

	public enum class BVP_algorithm
	{
		DiffApprox,
		Balance
	};

	//
	//	Managed wrapper for CircuitSolver
	//
	public ref class CircuitModel
	{
	public:
		CircuitModel(Double Ck, Double Lk, Double Rk, Double U0, Double I0, Double p0, Double T0,
			Double radius, Double length, Double dt, Double t_max, Boolean adaptive, SolverAlgorithm alg)
		{
			static int instance_count = 1;

			this->Ck = Ck;
			this->Lk = Lk;
			this->Rk = Rk;
			this->U0 = U0;
			this->I0 = I0;
			this->p0 = p0;
			this->T0 = T0;
			this->radius = radius;
			this->length = length;
			this->dt = dt;
			this->t_max = t_max;
			this->adaptive = adaptive;
			this->alg = alg;
			color = Color::FromArgb(std::rand() % 256, std::rand() % 256, std::rand() % 256);
			solution_name = gcnew String("������� " + instance_count.ToString());
			instance_count++;

			Time = gcnew List<double>();
			Time2 = gcnew List<double>();
			I = gcnew List<double>();
			U = gcnew List<double>();
			R = gcnew List<double>();
			P = gcnew List<double>();
		}

		//
		// Public properties
		//

		static CircuitModel ^createDefaultModel();

		[Description("��� �������"), Category("������������")]
		[DisplayNameAttribute("���")]
		property String ^solution_name;

		[Description("���� �������"), Category("������������")]
		[DisplayNameAttribute("����")]
		property Color color;
		
		[Description("������������� ������������ [�]"), Category("���������")]
		property Double Ck;				// capacitor

		[Description("������������� ������� [��]"), Category("���������")]
		property Double Lk;				// circuit inductivity

		[Description("���������� ������������� ������� ��� ����� ����� [��]"), Category("���������")]
		property Double Rk;				// circuit electrical resistance

		[Description("��������� ���������� [�]"), Category("���������")]
		property Double U0;				// initial voltage

		[Description("��������� ���� ���� [�]"), Category("���������")]
		property Double I0;				// initial current

		[Description("�������� � ����� � ��������� �������� [��]"), Category("���������")]
		property Double p0;				// lamp pressure in rest

		[Description("����������� � ����� � ��������� �������� [�]"), Category("���������")]
		property Double T0;				// lamp gas temperature in rest

		[Description("������ ����� [��]"), Category("���������")]
		property Double radius;			// lamp radius

		[Description("����� ����� [��]"), Category("���������")]
		property Double length;			// lamp length

		[Description("��� �������������� [�]"), Category("����������")]
		property Double dt;				// delta-t

		[Description("������ �������������� [�]"), Category("����������")]
		property Double t_max;			// maximum time

		[Description("������������ �� ���������� ���, ���� ��� ��������?"), Category("����������")]
		property Boolean adaptive;		// adaptive delta-t

		[Description("������������ �������� ������� ������ ����"), Category("����������")]
		property SolverAlgorithm alg;	// algorithm kind

		// thread-safe main function wrapper
		[BrowsableAttribute(false)]
		void calculate()
		{
			mutex->WaitOne();
			_calculate();
			mutex->ReleaseMutex();
		}

		//
		// Binding sources for result tables
		//
		[BrowsableAttribute(false)]
		property List<double> ^Time;

		[BrowsableAttribute(false)]
		property List<double> ^I;

		[BrowsableAttribute(false)]
		property List<double> ^U;

		[BrowsableAttribute(false)]
		property List<double> ^Time2;

		[BrowsableAttribute(false)]
		property List<double> ^R;

		[BrowsableAttribute(false)]
		property List<double> ^P;

		//
		// Debug methods
		//
		static Tuple<List<Double> ^, List<Double> ^> ^plotTempareture(double _I, double _Tw);
		static Tuple<List<Double> ^, List<Double> ^> ^plotPressure(double imin, double imax);
		static Tuple<List<Double> ^, List<Double> ^> ^plotResistance(double imin, double imax, double length, double radius);
		static Tuple<List<Double> ^, List<Double> ^> ^plotSpectre(double freqmin, double freqmax, double pressure, double temp);
		static Tuple<List<Double> ^, List<Double> ^> ^plotFlux(int divisions, double pressure, double _I, double radius, BVP_algorithm method);

	private:
		List<double> ^convert(std::vector<double> &vect);

		System::Threading::Mutex ^mutex = gcnew System::Threading::Mutex();

		void _calculate();
	};

}
