#pragma once
#include <math.h> 
#include <iostream> 

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

	Vec2& Normalize()
	{
		float length = Length();
		if (length != 0.f) 
			*this = *this / length;
		return *this;
	}

	Vec2& Rotate(float degree)
	{
		float radian = degree * (float)M_PI / 180.f;
		float x2 = x * cos(radian) - y * sin(radian);
		float y2 = x * sin(radian) + y * cos(radian);
		return Vec2(x2,y2);
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

