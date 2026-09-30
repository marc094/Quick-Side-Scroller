#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include "Timer.h"
#include "Rect.h"
#include "vec2.h"
#include "QuadTree.h"
#include <list>
#include <random>
#include <vector>

struct SDL_Window;
struct SDL_Renderer;
class Camera;
class PhysBody;

class Application
{
public:
	Application();
	virtual ~Application();

	void InitBody(PhysBody*);

	void Start();
	bool CheckInput();
	void PreUpdate();
	void Update();
	void Draw();
	void Reset();

	void MergeBodies(PhysBody& survivor, PhysBody& absorbed);
	void ResolveCollisions();
	void ComputeForces();
	void AccumulateForcesDirect();
	void Step(scalar dt);
	void UpdateTrail(bool advance);
	void Finish();

	void SetPaused(bool value) { paused = value; }
	void PrintStats() const;
	void PrintForceError();
	void SetExactForces(bool value) { exactForces = value; }

	svec2 GenerateInitialPosition();

	PhysBody* GetHeaviest();

private:
	std::mt19937_64 rng;

	Camera * camera = null;
	SDL_Window* window = null;
	SDL_Renderer* renderer = null;
	PhysBody* rocks = nullptr;
	Timer frameTimeTimer;
	bool G_FORCE = false;
	scalar timescale = INITIAL_TIME_SCALE;
	bool reset = false;
	iRect selectionRect = { 0,0,0,0 };
	bool drawSelectionRect = false;
	bool movementDrag = false;
	uint totalActiveBodies = 0u;

	// Trail of the camera target only (kept out of PhysBody to avoid ~50KB per body)
	std::vector<svec2> trail;
	std::vector<SDL_FPoint> trailScreen;
	PhysBody* trailOwner = nullptr;
	int trailIndex = 0;
	scalar trailTimer = 0;

	QuadTree tree;
	std::vector<int> candidates;
	bool treeValid = false;
	bool exactForces = false;

	// False when `force` doesn't reflect the current state (start, reset, respawn)
	bool forcesValid = false;

	bool paused = true;
	bool doStep = false;
};

#endif