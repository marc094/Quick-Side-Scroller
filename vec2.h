#ifndef __VEC2_H__
#define __VEC2_H__

#include "Defs.h"
#include "SDL\include\SDL_rect.h"

template<class TYPE>
struct vec2
{
	vec2() {}
	vec2(TYPE x, TYPE y) :x(x), y(y) {}
	explicit vec2(SDL_FPoint p) :x(p.x), y(p.y) {}

	TYPE x, y;

	explicit operator SDL_FPoint() const { return { (float)x, (float)y }; }

	vec2 vec2::operator +(const vec2& other) const {
		vec2 n(x + other.x, y + other.y);
		return n;
	}

	vec2 vec2::operator -(const vec2& other) const {
		vec2 n(x - other.x, y - other.y);
		return n;
	}

	vec2 vec2::operator *(TYPE sc) const
	{
		svec2 n(x * sc, y * sc);
		return n;
	}

	vec2 vec2::operator/(TYPE sc) const
	{
		svec2 n(x / sc, y / sc);
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

	const vec2& vec2::operator*=(TYPE sc) {
		x *= sc;
		y *= sc;
		return (*this);
	}

	const vec2 & vec2::operator/=(TYPE sc)
	{
		x /= sc;
		y /= sc;
		return (*this);
	}

	scalar vec2::distance(const vec2& other) const
	{
		return sqrt((other.x - x) * (other.x - x) + (other.y - y) * (other.y - y));
	}

	static scalar vec2::sDistance(const vec2& a, const vec2& b)
	{
		return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
	}

	scalar vec2::sqrDistance(const vec2& other) const {
		return (other.x - x) * (other.x - x) + (other.y - y) * (other.y - y);
	}

	scalar vec2::length() const {
		return sqrt(x * x + y * y);
	}

	scalar vec2::sqrLength() const {
		return x * x + y * y;
	}

	scalar vec2::angle(const vec2& other) const {
		return atan2(y - other.y, x - other.x);
	}

	vec2 vec2::normalised() const
	{
		return vec2(x, y) / length();
	}

	void vec2::normalise()
	{
		scalar l = max(length(), .0000000000000000001);
		x /= l;
		y /= l;
	}
};

typedef vec2<scalar> svec2;
typedef vec2<int> ivec2;

const static svec2 sZero = { 0.0,0.0 };
const static ivec2 iZero = { 0,0 };

#endif