#include "PhysBody.h"

extern float deltaTime;

PhysBody::PhysBody()
{
}


PhysBody::~PhysBody()
{
}

void PhysBody::UpdateTrail()
{
	if (trailTimer > TRAIL_UPDATE_FREQUENCY)
	{
		trailTimer = 0.f;
		trailIndex--;
		if (trailIndex < 0)
			trailIndex = TRAIL_LENGTH - 1;

		trail[trailIndex] = pos;
	}
	trailTimer += deltaTime;
}

SDL_FPoint * PhysBody::sortedTrail()
{
	int index = trailIndex;

	for (int i = 0; i < TRAIL_LENGTH; i++)
	{
		sortedTrailArr[i] = (SDL_FPoint)trail[(trailIndex + i) % TRAIL_LENGTH];
	}

	return sortedTrailArr;
}
