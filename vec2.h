#ifndef __VEC2_H__
#define __VEC2_H__

template<class TYPE>
struct vec2
{
	vec2() {}
	vec2(TYPE x, TYPE y) :x(x), y(y) {}

	TYPE x, y;

	vec2 vec2::operator +(const vec2& other) const {
		vec2 n(x + other.x, y + other.y);
		return n;
	}

	vec2 vec2::operator -(const vec2& other) const {
		vec2 n(x - other.x, y - other.y);
		return n;
	}

	vec2 vec2::operator *(TYPE scalar) const
	{
		dvec2 n(x * scalar, y * scalar);
		return n;
	}

	vec2 vec2::operator/(TYPE scalar) const
	{
		dvec2 n(x / scalar, y / scalar);
		return n;
	}

	const vec2& vec2::operator +=(const vec2& other)
	{
		x += other.x;
		y += other.y;
		return(*this);
	}

	const vec2& vec2::operator -=(const vec2& other)
	{
		x -= other.x;
		y -= other.y;
		return(*this);
	}

	const vec2& vec2::operator*=(TYPE scalar) {
		x *= scalar;
		y *= scalar;
		return (*this);
	}

	const vec2 & vec2::operator/=(TYPE scalar)
	{
		x /= scalar;
		y /= scalar;
		return (*this);
	}

	double vec2::distance(const vec2& other) const {
		return sqrt((other.x - x) * (other.x - x) + (other.y - y) * (other.y - y));
	}

	double vec2::sqrDistance(const vec2& other) const {
		return (other.x - x) * (other.x - x) + (other.y - y) * (other.y - y);
	}

	double vec2::length() const {
		return sqrt(x * x + y * y);
	}

	double vec2::sqrLength() const {
		return x * x + y * y;
	}

	double vec2::angle(const vec2& other) const {
		return atan2(y - other.y, x - other.x);
	}

	vec2 vec2::normalised() const
	{
		return vec2(x, y) / length();
	}

	void vec2::normalise()
	{
		double l = length();
		x /= l;
		y /= l;
	}
};

typedef vec2<double> dvec2;
typedef vec2<int> ivec2;

#endif