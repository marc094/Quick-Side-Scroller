#ifndef __PHYSBODY_H__
#define __PHYSBODY_H__
#include "Utils.h"
#include "SDL\include\SDL_Rect.h"

class PhysBody
{
public:
	Circle circle;
	uint64 mass;
	scalar density;
	scalar diametre;
	scalar area;
	svec2 speed;
	svec2 pos;
	bool active = true;
	Color color;
	svec2 force;

	svec2 trail[TRAIL_LENGTH];
	SDL_FPoint sortedTrailArr[TRAIL_LENGTH];
	sint16 trailIndex = 0;
	uint8 trailRecordIndex = 0;
	float trailTimer = 0;

public:
	PhysBody();
	virtual ~PhysBody();

	void UpdateTrail();
	SDL_FPoint* sortedTrail();
};

#endif