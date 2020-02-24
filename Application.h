#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include "Timer.h"
#include "Rect.h"

struct SDL_Window;
struct SDL_Renderer;
typedef class Camera;
typedef class PhysBody;

class Application
{
public:
	Application();
	virtual ~Application();

	void InitBody(int i);

	void Start();
	bool CheckInput();
	void PreUpdate();
	void Update();
	void Draw();
	void Reset();
	void Finish();

private:
	Camera * camera = nullptr;
	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;
	PhysBody* rocks = nullptr;
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