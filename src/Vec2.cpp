#include "Vec2.h"

#include <cmath>

// ------------- Constructors/Destructor ------------- //

Vec2::Vec2()
	: x(0), y(0)
{}

Vec2::Vec2(float x, float y)
	: x(x), y(y)
{}

Vec2::~Vec2() = default;

// ---------------- Class functions ------------------ //

float Vec2::Dot(const Vec2& other) const
{
	return x * other.x + y * other.y;
}

float Vec2::Dot(const Vec2& a, const Vec2& b)
{
	return a.x * b.x + a.y * b.y;
}

float Vec2::Length() const
{
	return std::sqrt(x * x + y * y);
}

float Vec2::LengthSquared() const
{
	return Dot(*this, *this);
}

Vec2 Vec2::Normalized() const
{
	float len = Length();

	if (len > 0)
	{
		return Vec2(x / len, y / len);
	}

	return Vec2::Zero();
}

void Vec2::Normalize()
{
	float len = Length();

	if (len > 0)
	{
		x /= len;
		y /= len;
	}
}

// -------------------- Operators -------------------- //

Vec2 Vec2::operator+(const Vec2& other) const
{
	return Vec2(x + other.x, y + other.y);
}

Vec2 Vec2::operator-(const Vec2& other) const
{
	return Vec2(x - other.x, y - other.y);
}

Vec2 Vec2::operator/(const float scalar) const
{
	return Vec2(x / scalar, y / scalar);
}

Vec2 Vec2::operator*(const float scalar) const
{
	return Vec2(x * scalar, y * scalar);
}

// ---------- Compund assingment operators ----------- //

Vec2& Vec2::operator+=(const Vec2& other)
{
	x += other.x;
	y += other.y;

	return *this;
}

Vec2& Vec2::operator-=(const Vec2& other)
{
	x -= other.x;
	y -= other.y;

	return *this;
}

Vec2& Vec2::operator/=(const float scalar)
{
	x /= scalar;
	y /= scalar;

	return *this;
}

Vec2& Vec2::operator*=(const float scalar)
{
	x *= scalar;
	y *= scalar;

	return *this;
}

// -------------- Comparative operators -------------- //

bool Vec2::operator==(const Vec2& other) const
{
	return (x == other.x && y == other.y);
}

bool Vec2::operator!=(const Vec2& other) const
{
	return (x != other.x || y != other.y);
}