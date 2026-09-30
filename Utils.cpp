#include "Utils.h"

bool Utils::CircleInCircle(Circle c1, Circle c2)
{
	if (sqrt(pow(c1.x - c2.x, 2) + pow(c1.y - c2.y, 2)) < c1.radius + c2.radius)
		return true;
	return false;
}

svec2 Utils::MoveTowards(svec2 value, svec2 target, scalar step)
{
	svec2 totalDisplacement = target - value;
	svec2 displacement = totalDisplacement.normalised() * step;
	if (displacement.sqrLength() < totalDisplacement.sqrLength())
	{
		return value + displacement;
	}
	return target;
}

scalar Utils::MoveTowards(scalar value, scalar target, scalar step)
{
	scalar totalDisplacement = target - value;
	scalar displacement = step * (totalDisplacement / fabs(totalDisplacement));
	if (displacement < totalDisplacement)
	{
		return value + displacement;
	}
	return target;
}

int Utils::MoveTowards(int value, int target, scalar step)
{
	int totalDisplacement = target - value;
	scalar displacement = step * (totalDisplacement / fabs(totalDisplacement));
	if (displacement < totalDisplacement)
	{
		return value + displacement;
	}
	return target;
}

bool Utils::IntersectRect(const iRect & a, const iRect & b)
{
	if ((a.x + a.w < b.x) || (a.x > b.x + b.w) || (a.y + a.h < b.y) || (a.y > b.y + b.h))
	{
		return false;
	}
	return true;
}

scalar Utils::Clamp(scalar s, scalar minimum, scalar maximum)
{
	return max(min(s,maximum), minimum);
}

scalar Utils::Clamp01(scalar s)
{
	return max(min(s, 1), 0);
}
