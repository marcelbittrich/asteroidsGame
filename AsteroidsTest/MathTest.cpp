#include "pch.h"
#include "CppUnitTest.h"
#include "../Asteroids/src/vector2.hpp"
#include <math.h> 

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace MathTest
{
	TEST_CLASS(Vector2Test)
	{
	public:
		TEST_METHOD(Plus)
		{
			Vec2 result = Vec2(1.4f, 1.6f) + Vec2(3.2f, 0.6f);
			Vec2 expectedResult = Vec2(4.6f, 2.2f);

			Assert::AreEqual(expectedResult.x, result.x, FLT_TOLERANCE);
			Assert::AreEqual(expectedResult.y, result.y, FLT_TOLERANCE);
		}
		TEST_METHOD(PlusEqual)
		{
			Vec2 result = Vec2(1.4f, 1.6f);
			result += Vec2(3.2f, 0.6f);

			Vec2 expectedResult = Vec2(4.6f, 2.2f);

			Assert::AreEqual(expectedResult.x, result.x, FLT_TOLERANCE);
			Assert::AreEqual(expectedResult.y, result.y, FLT_TOLERANCE);
		}

		TEST_METHOD(Minus)
		{
			Vec2 result = Vec2(1.4f, 1.6f) - Vec2(3.2f, 0.6f);

			Vec2 expectedResult = Vec2(-1.8f, 1.0f);

			Assert::AreEqual(expectedResult.x, result.x, FLT_TOLERANCE);
			Assert::AreEqual(expectedResult.y, result.y, FLT_TOLERANCE);
		}

		TEST_METHOD(MinusEqual)
		{
			Vec2 result = Vec2(1.4f, 1.6f);
			result -= Vec2(3.2f, 0.6f);

			Vec2 expectedResult = Vec2(-1.8f, 1.0f);

			Assert::AreEqual(expectedResult.x, result.x, FLT_TOLERANCE);
			Assert::AreEqual(expectedResult.y, result.y, FLT_TOLERANCE);
		}

		TEST_METHOD(Multiply)
		{
			Vec2 result = Vec2(1.2f, 1.2f) * Vec2(2.5f, -2.5f);

			Vec2 expectedResult = Vec2(3.0f, -3.0f);

			Assert::AreEqual(expectedResult.x, result.x, FLT_TOLERANCE);
			Assert::AreEqual(expectedResult.y, result.y, FLT_TOLERANCE);
		}

		TEST_METHOD(Division)
		{
			Vec2 result = Vec2(3.2f, -3.2f) / 2.0f;

			Vec2 expectedResult = Vec2(1.6f, -1.6f);

			Assert::AreEqual(expectedResult.x, result.x, FLT_TOLERANCE);
			Assert::AreEqual(expectedResult.y, result.y, FLT_TOLERANCE);
		}

		TEST_METHOD(SquareLength)
		{
			float result = Vec2(3.f, -3.f).SquareLength();

			float expectedResult = 18.f;

			Assert::AreEqual(expectedResult, result, FLT_TOLERANCE);
		}

		TEST_METHOD(Length)
		{
			float result = Vec2(4.f, -4.0f).Length();

			float expectedResult = 5.6568542f;

			Assert::AreEqual(expectedResult, result, FLT_TOLERANCE);
		}

		TEST_METHOD(Normalize)
		{
			Vec2 vector = Vec2(1.f, 1.0f);
			Vec2 vectorBeforeNormalize = vector;
			Vec2 result = vector.Normalize();

			// Vector component devided by length of vector.
			// For this example: x = 1.f / sqrt(1.f * 1.f + 1.f * 1.f) 
			Vec2 expectedResult = Vec2(0.707106f, 0.707106f);

			Assert::AreEqual(expectedResult.x, result.x, FLT_TOLERANCE);
			Assert::AreEqual(expectedResult.y, result.y, FLT_TOLERANCE);

			Assert::AreEqual(vectorBeforeNormalize.x, vector.x, FLT_TOLERANCE);
			Assert::AreEqual(vectorBeforeNormalize.y, vector.y, FLT_TOLERANCE);
		}

		TEST_METHOD(Rotate)
		{
			Vec2 vector = Vec2(0.f, 1.0f);
			Vec2 result = vector.Rotate(90.f);

			Vec2 expectedResult = Vec2(-1.f, 0.f);

			Assert::AreEqual(expectedResult.x, result.x, FLT_TOLERANCE);
			Assert::AreEqual(expectedResult.y, result.y, FLT_TOLERANCE);
		}

	};
}
