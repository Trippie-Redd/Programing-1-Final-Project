#pragma once

class Vec2
{
public:
	float x, y;

public:
	Vec2();
	Vec2(float x, float y);
	~Vec2();

	float Dot(const Vec2& other) const;
	static float Dot(const Vec2& a, const Vec2& b);
	float Length() const;
	float LengthSquared() const;
	Vec2 Normalized() const;
	void Normalize();

	static float Cross(const Vec2& a, const Vec2& b) { return a.x * b.y - a.y * b.x; }

	Vec2 operator+(const Vec2& other)  const;
	Vec2 operator-(const Vec2& other)  const;
	Vec2 operator*(const float scalar) const;
	Vec2 operator/(const float scalar) const;

	Vec2& operator+=(const Vec2& other);
	Vec2& operator-=(const Vec2& other);
	Vec2& operator*=(const float scalar);
	Vec2& operator/=(const float scalar);

	bool operator==(const Vec2& other) const;
	bool operator!=(const Vec2& other) const;

	static Vec2 Zero()  { return Vec2(0, 0);  }
	static Vec2 One()   { return Vec2(1, 1);  }
	static Vec2 Up()    { return Vec2(0, 1);  }
	static Vec2 Down()  { return Vec2(0, -1); }
	static Vec2 Left()  { return Vec2(-1, 0); }
	static Vec2 Right() { return Vec2(1, 0);  }
};