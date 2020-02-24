#ifndef __DEFS_H__
#define __DEFS_H__

// Globals --------------------------------------------------------
#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
#define HALF_SCREEN_WIDTH 960
#define HALF_SCREEN_HEIGHT 540
#define MAX_BODIES 2500
#define MAX_MASS 1000000000000
#define MIN_MASS 100000000000
#define MIN_DIAMETRE 3
#define MAX_DIAMETRE 10
#define MAX_DENSITY 500000000000
#define MIN_DENSITY 50000000000
#define MAX_CIRCLE_POINTS 120
#define MAX_CAMERA_MOVEMENT_SPEED (scalar)16000000000
#define CAMERA_ACCELERATION (scalar)10
#define G_CONSTANT 0.00000000006673
#define INITIAL_TIME_SCALE 0.1
#define SPAWN_RADIUS 500
#define TRAIL_LENGTH 512
#define TRAIL_UPDATE_FREQUENCY 1

#define max(a,b) (a>b?a:b)
#define min(a,b) (a<b?a:b)

//#define USE_FLOAT

typedef unsigned int		uint;
typedef unsigned char		uint8;
typedef unsigned short		uint16;
typedef unsigned long		uint32;
typedef unsigned long long	uint64;

typedef signed int			sint;
typedef signed char			sint8;
typedef signed short		sint16;
typedef signed long			sint32;
typedef signed long long	sint64;

#ifdef USE_FLOAT
typedef float scalar;
#else
typedef double scalar;
#endif

#endif