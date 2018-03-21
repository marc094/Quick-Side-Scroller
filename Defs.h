#ifndef __DEFS_H__
#define __DEFS_H__

#include <math.h>
#include "SDL\include\SDL.h"
#include "vec2.h"

template<class TYPE>
struct Rect {
	TYPE x, y, w, h;

	Rect Normalised() {
		Rect r = (*this);
		if (w < 0 && h < 0)
			r = { x + w, y + h, -w, -h };
		else if (w < 0)
			r = { x + w, y, -w, h };
		else if (h < 0)
			r = { x, y + h, w, -h };
		return r;
	}

	SDL_Rect toSDL() const {
		return { (int)x, (int)y, (int)w, (int)h };
	}
};

typedef Rect<double> dRect;
typedef Rect<int> iRect;


struct Color {
	unsigned short int r, g, b;
};

struct Circle {
	Circle() {}
	Circle(int x, int y, int diametre) :x(x), y(y), radius((double)diametre / 2) {}
	int x, y;
	float radius;
};

struct CelestialBody {
	Circle circle;
	unsigned long long mass;
	double density;
	double diametre;
	double volume;
	dvec2 speed = dvec2(0, 0);
	dvec2 pos;
	bool active = true;
	Color color;
};

// Globals --------------------------------------------------------
#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
#define MAX_BODIES 1000
#define MAX_MASS 1000000000000
#define MIN_MASS 10000000000
#define MIN_DIAMETRE 3
#define MAX_DIAMETRE 3
#define CIRCLE_POINTS 60
#define CAMERA_MOVEMENT_SPEED (double)10

void Draw();
void Update();
bool CheckInput();
void Finish();
void Start();
void Reset();
void InitBody(int index);
bool CheckCollision(CelestialBody b1, CelestialBody b2);
bool CircleInCircle(Circle c1, Circle c2);
dvec2 Interpolate(dvec2 value, dvec2 target, double step);

template<class TYPE>
TYPE Interpolate(TYPE value, TYPE target, TYPE step);
#endif

template<class TYPE>
inline TYPE Interpolate(TYPE value, TYPE target, TYPE step)
{
	TYPE totalDisplacement = target - value;
	TYPE displacement = step * (totalDisplacement / abs(totalDisplacement));
	TYPE finalVector;
	if (displacement < totalDisplacement)
	{
		finalVector = value + displacement;
	}
	else finalVector = target;
	return finalVector;
}

template<class TYPE>
TYPE max(TYPE a, TYPE b) { return (a > b) ? a : b; }

template<class TYPE>
TYPE min(TYPE a, TYPE b) { return (a < b) ? a : b; }