#pragma once
#include <cmath>

constexpr float PI = 3.1415927f;

class Vec2
{
public:
	float x = 0, y = 0;

public:
	//-----------------------------------------------------------------------------
	// Constructors and destructor
	//-----------------------------------------------------------------------------
	Vec2(float x, float y) : x(x), y(y) {}
	Vec2() = default;
	~Vec2() = default;

	//-----------------------------------------------------------------------------
	// Class functions
	//-----------------------------------------------------------------------------
	float Dot(const Vec2& other)  const { return x * other.x + y * other.y; }
	float Length()                const { return std::sqrt(x * x + y * y);  }
	float LengthSquared()         const { return x * x + y * y;             }
	Vec2 Normalized() const {
		float length = Length();
		return length > 0 ? *this / length : Vec2::Zero();
	}
	void Normalize() { 
		float length = Length(); 
		if (length > 0) *this /= length; 
	}

	//-----------------------------------------------------------------------------
	// Static functions
	//-----------------------------------------------------------------------------
	static float Dot(const Vec2& a, const Vec2& b)    { return a.x * b.x + a.y * b.y; }
	static float Cross(const Vec2& a, const Vec2& b)  { return a.x * b.y - a.y * b.x; }
	
	//-----------------------------------------------------------------------------
	// Operators
	//-----------------------------------------------------------------------------
	Vec2 operator+(const Vec2& other)  const { return Vec2(x + other.x, y + other.y); }
	Vec2 operator-(const Vec2& other)  const { return Vec2(x - other.x, y - other.y); }
	Vec2 operator*(const float scalar) const { return Vec2(x * scalar, y * scalar);   }
	Vec2 operator/(const float scalar) const { return Vec2(x / scalar, y / scalar);   }

	//-----------------------------------------------------------------------------
	// Compund assingment operators
	//-----------------------------------------------------------------------------
	Vec2& operator+=(const Vec2& other)   { x += other.x; y += other.y; return *this; }
	Vec2& operator-=(const Vec2& other)   { x -= other.x; y -= other.y; return *this; }
	Vec2& operator*=(const float scalar)  { x *= scalar; y *= scalar; return *this;   }
	Vec2& operator/=(const float scalar)  { x /= scalar; y /= scalar; return *this;   }

	//-----------------------------------------------------------------------------
	// Comparative operators
	//-----------------------------------------------------------------------------
	bool operator==(const Vec2& other) const { return (x == other.x && y == other.y); }
	bool operator!=(const Vec2& other) const { return (x != other.x || y != other.y); }

	//-----------------------------------------------------------------------------
	// Static utility functions
	//-----------------------------------------------------------------------------
	static Vec2 Zero()  { return Vec2(0, 0);  }
	static Vec2 One()   { return Vec2(1, 1);  }
	static Vec2 Up()    { return Vec2(0, 1);  }
	static Vec2 Down()  { return Vec2(0, -1); }
	static Vec2 Left()  { return Vec2(-1, 0); }
	static Vec2 Right() { return Vec2(1, 0);  }
};