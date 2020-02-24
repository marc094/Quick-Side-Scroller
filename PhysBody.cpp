#include "PhysBody.h"


PhysBody::PhysBody()
{
}


PhysBody::~PhysBody()
{
}

void PhysBody::UpdateTrail()
{
	if (trailRecordIndex % TRAIL_UPDATE_FREQUENCY == 0)
	{
		trailIndex--;
		if (trailIndex < 0)
			trailIndex = TRAIL_LENGTH - 1;

		trail[trailIndex] = pos;
	}
	trailRecordIndex++;
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
