#pragma once
#include <cmath>

constexpr float PI = 3.1415927f;

// ts fire
class Vec2
{
public:
	float x = 0, y = 0;

public:
	//-----------------------------------------------------------------------------
	// Constructors and destructor
	//-----------------------------------------------------------------------------
	constexpr Vec2(float x, float y) : x(x), y(y) {}
	constexpr Vec2() = default;
	~Vec2() = default;

	//-----------------------------------------------------------------------------
	// Class functions
	//-----------------------------------------------------------------------------
	
	constexpr float Dot(const Vec2& other)  const { return x * other.x + y * other.y; }
	constexpr float LengthSquared()         const { return x * x + y * y;             }
	inline float Length()                   const { return std::sqrt(x * x + y * y);  }
	inline Vec2 Normalized() const {
		float length = Length();
		return length > 0 ? *this / length : Vec2::Zero();
	}
	inline void Normalize() {
		float length = Length(); 
		if (length > 0) *this /= length; 
	}

	//-----------------------------------------------------------------------------
	// Static functions
	//-----------------------------------------------------------------------------
	static constexpr float Dot(const Vec2& a, const Vec2& b)    { return a.x * b.x + a.y * b.y; }
	static constexpr float Cross(const Vec2& a, const Vec2& b)  { return a.x * b.y - a.y * b.x; }
	
	//-----------------------------------------------------------------------------
	// Operators
	//-----------------------------------------------------------------------------
	constexpr Vec2 operator+(const Vec2& other)  const { return Vec2(x + other.x, y + other.y); }
	constexpr Vec2 operator-(const Vec2& other)  const { return Vec2(x - other.x, y - other.y); }
	constexpr Vec2 operator*(const float scalar) const { return Vec2(x * scalar, y * scalar);   }
	constexpr Vec2 operator/(const float scalar) const { return Vec2(x / scalar, y / scalar);   }

	//-----------------------------------------------------------------------------
	// Compund assingment operators
	//-----------------------------------------------------------------------------
	constexpr Vec2& operator+=(const Vec2& other)   { x += other.x; y += other.y; return *this; }
	constexpr Vec2& operator-=(const Vec2& other)   { x -= other.x; y -= other.y; return *this; }
	constexpr Vec2& operator*=(const float scalar)  { x *= scalar; y *= scalar; return *this;   }
	constexpr Vec2& operator/=(const float scalar)  { x /= scalar; y /= scalar; return *this;   }

	//-----------------------------------------------------------------------------
	// Comparative operators
	//-----------------------------------------------------------------------------
	constexpr bool operator==(const Vec2& other) const { return (x == other.x && y == other.y); }
	constexpr bool operator!=(const Vec2& other) const { return (x != other.x || y != other.y); }

	//-----------------------------------------------------------------------------
	// Static utility functions
	//-----------------------------------------------------------------------------
	static constexpr Vec2 Zero()  { return Vec2(0, 0);  }
	static constexpr Vec2 One()   { return Vec2(1, 1);  }
	static constexpr Vec2 Up()    { return Vec2(0, 1);  }
	static constexpr Vec2 Down()  { return Vec2(0, -1); }
	static constexpr Vec2 Left()  { return Vec2(-1, 0); }
	static constexpr Vec2 Right() { return Vec2(1, 0);  }
};