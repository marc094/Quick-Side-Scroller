#ifndef __UTILS_H__
#define __UTILS_H__

#include "Defs.h"
#include "vec2.h"
#include <cmath>

struct Color
{
	uint8 r, g, b;
};

struct Circle
{
	Circle() {}
	Circle(int x, int y, int diametre) :x(x), y(y), radius((scalar)diametre / 2) {}
	int x, y;
	scalar radius;
};

namespace Utils
{
	bool CircleInCircle(Circle c1, Circle c2);
	svec2 Interpolate(svec2 value, svec2 target, scalar step);
	scalar Interpolate(scalar value, scalar target, scalar step);
	int Interpolate(int value, int target, scalar step);
}
#endif
