#include "stdafx.h"
#include "CppUnitTest.h"
#include "../SolverLib/Interpolation.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace NativeUnitTest
{		
	TEST_CLASS(UnitTest1)
	{
	public:
		
        TEST_METHOD(InterpolationTest)
        {
            std::vector<double> axis = { 0.0, 1.0 };
            std::vector<double> data = { 0.0, 1.0 };
            Axis_t axis_set = Axis_t(axis, InterpType::Interp);
            double result = Lerp<0>::interpolate({ axis_set }, data, { 0.5 });
            Assert::AreEqual(0.5, result, 1e-6, L"Basic 0.0 1.0 linear 1-dimentional interpolation");
		}

	};
}