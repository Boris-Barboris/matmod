#pragma once

#include "Stdafx.h"

enum InterpType	{ Interp, ConstInterval };

struct Axis_t
{
	std::vector<double> pts;
	InterpType type;
	Axis_t(std::vector<double> axis, InterpType type) : pts(axis), type(type) {}
};

typedef std::vector<Axis_t> Axis_set;

///////////////////////////////////////////////////////////////////////////////////
// N-dimensional linear interpolation\extrapolation
// level = N-1
///////////////////////////////////////////////////////////////////////////////////
template <int level> struct Lerp
{
	typedef std::vector<typename Lerp<level - 1>::data_t> data_t;		//typedef for data table type

	// lineary interpolate n-dimensional function in point x by table data
	// axis-set - vector of Axis_t
	// data - level-dimensional table
	// x - point of entrance
	static double interpolate(const Axis_set &axis_set,
		const std::vector<data_t> &data, std::vector<double> x)
	{
		int i = 0;

		// find cell of x on axes_set[level] axis
		// it will be i and i + 1 slices of data
		if (!(x[level] < axis_set[level].pts[0]))
			while (!((x[level] >= axis_set[level].pts[i]) && (x[level] <= axis_set[level].pts[i + 1])))
			{
				i++;
				if (i == axis_set[level].pts.size() - 1)
				{
					i = axis_set[level].pts.size() - 2;
					break;
				}
			}

		switch (axis_set[level].type)
		{
			case InterpType::Interp:
			{
				double data1 = Lerp<level - 1>::interpolate(axis_set, data[i], x);		// recursively get interpolated value in i-slice
				double data2 = Lerp<level - 1>::interpolate(axis_set, data[i + 1], x);	// recursively get interpolated value in (i+1)-slice

				double dy = data2 - data1;
				double dx = (x[level] - axis_set[level].pts[i]);
				double h = (axis_set[level].pts[i + 1] - axis_set[level].pts[i]);

				double res = data1 + dy * dx / h;

				return res;
			}
			case InterpType::ConstInterval:
			{
				double data1 = Lerp<level - 1>::interpolate(axis_set, data[i], x);	// recursively get interpolated value in i-slice
				return data1;
			}
			default:
				return 0.0;
		}

	}
};

///////////////////////////////////////////////////////////////////////////////////
// 1-dimensional linear interpolation\extrapolation
///////////////////////////////////////////////////////////////////////////////////
template<> struct Lerp<0>
{
	typedef double data_t;		// in 1-dimensional linear Lerp, data - vector of doubles

	// lineary interpolate function in point x by table data
	// axis_set - vector of 1 Axis_t
	// data - vector of doubles, data table itself
	// x - point of entrance
	static double interpolate(const Axis_set &axis_set,
		const std::vector<data_t> &data, std::vector<double> x)
	{
		int i = 0;

		// find cell of x on axis
		// it will be i and i + 1 slices of data
		if (!(x[0] < axis_set[0].pts[0]))
			while (!((x[0] >= axis_set[0].pts[i]) && (x[0] <= axis_set[0].pts[i + 1])))
			{
				i++;
				if (i == axis_set[0].pts.size() - 1)
				{
					i = axis_set[0].pts.size() - 2;
					break;
				}
			}
		
		switch (axis_set[0].type)
		{
			case InterpType::Interp:
				return data[i] + (data[i + 1] - data[i]) *
					(x[0] - axis_set[0].pts[i]) / (axis_set[0].pts[i + 1] - axis_set[0].pts[i]);
			case InterpType::ConstInterval:
				return data[i];
			default:
				return 0.0;
		}
		
	}
};