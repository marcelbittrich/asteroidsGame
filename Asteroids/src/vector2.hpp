#pragma once
#include <math.h> 
#include <iostream> 

#ifndef M_PI
	#define M_PI 3.14159265358979323846
#endif

struct Vec2
{
	float x, y;

	Vec2()
		: x(0), y(0) {}

	Vec2(float x)
		: x(x), y(x) {}

	Vec2(float x, float y)
		: x(x), y(y) {}

	Vec2 Add(const Vec2& other)
	{
		return Vec2(x + other.x, y + other.y);
	}

	Vec2 operator+(const Vec2& other)
	{
		return Add(other);
	}

	Vec2& operator+=(const Vec2& other)
	{
		x += other.x;
		y += other.y;
		return *this;
	}

	Vec2 Subtract(const Vec2& other)
	{
		return Vec2(x - other.x, y - other.y);
	}

	Vec2 operator-(const Vec2& other)
	{
		return Subtract(other);
	}

	Vec2& operator-=(const Vec2& other)
	{
		x -= other.x;
		y -= other.y;
		return *this;
	}

	Vec2 Multiply(const float& scalar)
	{
		return Vec2(x * scalar, y * scalar);
	}

	Vec2 operator*(const float& scalar)
	{
		return Multiply(scalar);
	}

	// Element-wise
	Vec2 Multiply(const Vec2& other)
	{
		return Vec2(x * other.x, y * other.y);
	}

	Vec2 operator*(const Vec2& other)
	{
		return Multiply(other);
	}

	// Element-wise
	Vec2 Division(const float& scalar)
	{
		return Vec2(x / scalar, y / scalar);
	}

	Vec2 operator/(const float& scalar)
	{
		return Division(scalar);
	}

	float SquareLength()
	{
		return x * x + y * y;
	}

	float Length()
	{
		return sqrt(SquareLength());
	}

	// Returnes a normalized Vector.
	// Does not change the original Vector.
	// Result has to be anssigned.
	Vec2 Normalize()
	{
		float length = Length();
		return (length != 0.f) ? (*this / length) : Vec2(0.f, 0.f);
	}

	// Rotates vector clockwise.
	// Does not change the original Vector.
	// Result has to be anssigned.
	Vec2 Rotate(float degree)
	{
		float radian = degree * (float)M_PI / 180.f;
		float x2 = this->x * cos(radian) - this->y * sin(radian);
		float y2 = this->x * sin(radian) + this->y * cos(radian);
		return Vec2(x2, y2);
	}

	Vec2& SetLength(float length)
	{
		*this = this->Normalize();
		*this = *this * length;
		return *this;
	}

	static float Dot(const Vec2& vector1, const Vec2& vector2)
	{
		return vector1.x * vector2.x + vector1.y * vector2.y;
	}

	static float SquareDistance(const Vec2& point1, const Vec2& point2)
	{
		return (point1.x - point2.x) * (point1.x - point2.x)
			+ (point1.y - point2.y) * (point1.y - point2.y);
	}

	static float Distance(const Vec2& point1, const Vec2& point2)
	{
		return sqrt(SquareDistance(point1, point2));
	}

	friend std::ostream& operator<<(std::ostream& stream, const Vec2& other)
	{
		stream << other.x << ", " << other.y;
		return stream;
	}

};

