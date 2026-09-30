#ifndef __DEFS_H__
#define __DEFS_H__

#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif
#include <algorithm>
#include <cmath>
#include <list>
#include <random>
#include <string>
#include <type_traits>
#include <vector>

// Globals --------------------------------------------------------
#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
#define HALF_SCREEN_WIDTH 960
#define HALF_SCREEN_HEIGHT 540
#ifndef MAX_BODIES
#define MAX_BODIES 5000
#endif
#define MAX_MASS 10000000000
#define MIN_MASS 100000000
#define MAX_DENSITY 500000000
#define MIN_DENSITY 5000000
#define MAX_CIRCLE_POINTS 360
#define DRAW_BATCH_SIZE 65536
#define MAX_CAMERA_MOVEMENT_SPEED (scalar)16000000000
#define CAMERA_ACCELERATION (scalar)10
#define G_CONSTANT 0.00000000006673
#define INITIAL_TIME_SCALE 1
#define SPAWN_RADIUS (sint)5000
#define TRAIL_LENGTH 2048
#define TRAIL_UPDATE_FREQUENCY 20
#define CAMERA_CULLING_MARGIN 0
#define VELOCITY_VECTOR_SCALE 3
#define G_FORCE_STRENGTH 100000000000.
#define MAX_PHYSICS_STEP 1.0
#define MAX_SUBSTEPS 4
#define BH_THETA 0.5
#define MIN_INITIAL_MOMENTUM 10000000
#define MAX_INITIAL_MOMENTUM 1000000000

#define null 0

enum InitialDistribution
{
	RADIAL_CENTRE_DENSER,
	RADIAL_UNIFORM,
	SQUARE_CENTRE_DENSER,
	SQUARE_UNIFORM,
};

#define INITIAL_DISTRIBUTION RADIAL_CENTRE_DENSER

// Not macros: those break std::min/std::max. Mixed argument types are allowed, as with the old macros.
template<class A, class B> inline auto max(A a, B b) -> typename std::common_type<A, B>::type { return a > b ? a : b; }
template<class A, class B> inline auto min(A a, B b) -> typename std::common_type<A, B>::type { return a < b ? a : b; }

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

#define ARR_DECL(type, capacity) new type[capacity]
#define ARR_FREE(pointer) delete[](pointer)


#endif