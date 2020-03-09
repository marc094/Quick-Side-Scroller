#include "Utils.h"

bool Utils::CircleInCircle(Circle c1, Circle c2)
{
	if (sqrt(pow(c1.x - c2.x, 2) + pow(c1.y - c2.y, 2)) < c1.radius + c2.radius)
		return true;
	return false;
}

svec2 Utils::Interpolate(svec2 value, svec2 target, scalar step)
{
	svec2 totalDisplacement = target - value;
	svec2 displacement = totalDisplacement.normalised() * step;
	svec2 finalVector;
	if (displacement.sqrLength() < totalDisplacement.sqrLength())
	{
		finalVector = value + displacement;
	}
	else finalVector = target;
	return finalVector;
}

scalar Utils::Interpolate(scalar value, scalar target, scalar step)
{
	scalar totalDisplacement = target - value;
	scalar displacement = step * (totalDisplacement / abs(totalDisplacement));
	scalar finalVector;
	if (displacement < totalDisplacement)
	{
		finalVector = value + displacement;
	}
	else finalVector = target;
	return finalVector;
}

int Utils::Interpolate(int value, int target, scalar step)
{
	int totalDisplacement = target - value;
	scalar displacement = step * (totalDisplacement / abs(totalDisplacement));
	int final;
	if (displacement < totalDisplacement)
	{
		final = value + displacement;
	}
	else final = target;
	return final;
}

bool Utils::IntersectRect(const iRect & a, const iRect & b)
{
	if ((a.x + a.w < b.x) || (a.x > b.x + b.w) || (a.y + a.h < b.y) || (a.y > b.y + b.h))
	{
		return false;
	}
	return true;
	/*if ((a.x >= b.x && a.x <= b.x + b.w && a.y >= b.y && a.y <= b.y + b.h)
		|| (b.x >= a.x && b.x <= a.x + a.w && b.y >= a.y && b.y <= a.y + a.h))
	{
		return true;
	}
	return false;*/
}
