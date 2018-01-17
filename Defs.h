#ifndef __DEFS_H__
#define __DEFS_H__
#include <math.h>
#include "SDL\include\SDL.h"


struct rectangle {
	int x, y, w, h;
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
	float distance(const vec2& other);
	float angle(const vec2& other);
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
	unsigned long mass;
	double density;
	double diametre;
	vec2 speed = vec2(0, 0);
	vec2 pos;
	bool active = true;
	Color color;
};

// Globals --------------------------------------------------------
#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
#define MAX_BODIES 100
#define MAX_MASS 100000000000
#define MIN_MASS 10000000
#define MIN_DIAMETRE 1
#define MAX_DIAMETRE 10

void Draw();
void Update();
bool CheckInput();
void Finish();
void Start();
void InitBody(int index);
bool CheckCollision(CelestialBody b1, CelestialBody b2);
bool CircleInCircle(Circle c1, Circle c2);
#endif