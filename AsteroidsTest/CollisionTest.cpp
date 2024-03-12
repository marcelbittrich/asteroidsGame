#include "pch.h"
#include "CppUnitTest.h"
#include "src/objects/gameobjects/gameobject.hpp"
#include "src/physics/collisionhandler.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace CollisionTest
{
	TEST_CLASS(CollisionTest)
	{
	public:
		TEST_METHOD(MoveApartTest)
		{
			Vec2 midPos1 = { 0, 0 };
			Vec2 midPos2 = { 10, 0 };
			float colRadius1 = 7;
			float colRadius2 = 4;

			GameObject obj1(midPos1, Vec2(0,0));
			obj1.SetColRadius(colRadius1);

			GameObject obj2(midPos2, Vec2(0, 0));
			obj2.SetColRadius(colRadius2);

			CollisionHandler collisionHandler;
			collisionHandler.MoveApart(obj1, obj2);

			Vec2 expectedNewMidPos1 = { -0.5,0 };
			Vec2 expectedNewMidPos2 = { 10.5,0 };

			Assert::AreEqual(expectedNewMidPos1.x, obj1.GetMidPos().x, FLT_TOLERANCE);
			Assert::AreEqual(expectedNewMidPos1.y, obj1.GetMidPos().y, FLT_TOLERANCE);

			Assert::AreEqual(expectedNewMidPos2.x, obj2.GetMidPos().x, FLT_TOLERANCE);
			Assert::AreEqual(expectedNewMidPos2.y, obj2.GetMidPos().y, FLT_TOLERANCE);
		}

	};
}