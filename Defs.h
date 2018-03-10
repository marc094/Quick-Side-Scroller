#ifndef __DEFS_H__
#define __DEFS_H__

#include <math.h>
#include "SDL\include\SDL.h"


struct Rectangle {
	double x, y, w, h;
};

struct vec2
{
	vec2() {}
	vec2(double x, double y) :x(x), y(y) {}

	double x, y;

	const vec2& operator +(const vec2& other);
	const vec2& operator -(const vec2& other);
	const vec2& operator +=(const vec2& other);
	const vec2& operator *(double scalar);
	const vec2& operator *=(double scalar);
	const vec2& operator /(double scalar);

	double distance(const vec2& other) const;
	double length() const;
	double angle(const vec2& other) const;
	vec2 normalise() const;
};

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
	vec2 speed = vec2(0, 0);
	vec2 pos;
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
vec2 Interpolate(vec2 value, vec2 target, double step);

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
