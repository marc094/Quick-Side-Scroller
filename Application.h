#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include "Timer.h"
#include "Rect.h"
#include "vec2.h"
#include <list>
#include <random>

struct SDL_Window;
struct SDL_Renderer;
typedef class Camera;
typedef class PhysBody;

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
	void Finish();

	svec2 GenerateInitialPosition();

	PhysBody* GetHeaviest();

private:
	std::mt19937_64 rng;

	Camera * camera = null;
	SDL_Window* window = null;
	SDL_Renderer* renderer = null;
	PhysBody* rocks = nullptr;
	//std::list<PhysBody*> rocks;
	//std::list<PhysBody*> trash;
	Timer frameTimeTimer;
	bool G_FORCE = false;
	scalar timescale = INITIAL_TIME_SCALE;
	bool reset = false;
	iRect selectionRect = { 0,0,0,0 };
	bool drawSelectionRect = false;
	bool movementDrag = false;
	uint totalActiveBodies = 0u;

	bool paused = true;
	bool doStep = false;
};

#endif