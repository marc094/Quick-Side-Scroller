#ifndef __PHYSBODY_H__
#define __PHYSBODY_H__
#include "Utils.h"

class PhysBody
{
public:
	Circle circle;
	unsigned long long mass;
	scalar density;
	scalar diametre;
	scalar area;
	svec2 speed;
	svec2 pos;
	bool active = true;
	Color color;
	svec2 force;

public:
	PhysBody();
	virtual ~PhysBody();
};

#endif