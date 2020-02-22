#ifndef __DEFS_H__
#define __DEFS_H__

// Globals --------------------------------------------------------
#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
#define HALF_SCREEN_WIDTH 960
#define HALF_SCREEN_HEIGHT 540
#define MAX_BODIES 5000
#define MAX_MASS 1000000000000
#define MIN_MASS 100000000000
#define MIN_DIAMETRE 3
#define MAX_DIAMETRE 10
#define MAX_DENSITY 500000000000
#define MIN_DENSITY 50000000000
#define CIRCLE_POINTS 120
#define MAX_CAMERA_MOVEMENT_SPEED (scalar)540
#define CAMERA_ACCELERATION (scalar)1
#define G_CONSTANT 0.00000000006673
#define INITIAL_TIME_SCALE 0
#define SPAWN_RADIUS 1000
#define FORCE_FACTOR_DRAW 40

#define max(a,b) (a>b?a:b)
#define min(a,b) (a<b?a:b)

#define USE_FLOAT

typedef unsigned int uint;
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long uint64;

#ifdef USE_FLOAT
typedef float scalar;
#else
typedef double scalar;
#endif

#endif