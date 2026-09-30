#ifndef __MAIN__
#define __MAIN__
//#include "SDL_image\include\SDL_image.h"
//#include "SDL_mixer\include\SDL_mixer.h"

#ifdef _MSC_VER
#ifdef _WIN64
#pragma comment( lib, "SDL/lib/x64/SDL2.lib" )
#pragma comment( lib, "SDL/lib/x64/SDL2main.lib" )
#else
#pragma comment( lib, "SDL/lib/x86/SDL2.lib" )
#pragma comment( lib, "SDL/lib/x86/SDL2main.lib" )
#endif
#endif
/*#pragma comment( lib, "SDL_image/libx86/SDL2_image.lib" )
#pragma comment( lib, "SDL_mixer/libx86/SDL2_mixer.lib" )*/

/*#ifdef main
#undef main
#endif*/

#include "Application.h"
#include "Platform.h"
#include <cstdlib>
#include <cstring>

float deltaTime = 0.0f;
float realDeltaTime = 0.0f;

// ----------------------------------------------------------------
int main(int argc, char *args[])
{
	// --run: start unpaused. --frames N: exit after N frames and print conservation stats.
	// --exact: O(n^2) forces instead of Barnes-Hut. --check-error: print tree vs exact force error at start.
	bool run = false;
	bool exact = false, checkError = false;
	long maxFrames = -1;
	for (int i = 1; i < argc; ++i)
	{
		if (strcmp(args[i], "--run") == 0)
			run = true;
		else if (strcmp(args[i], "--exact") == 0)
			exact = true;
		else if (strcmp(args[i], "--check-error") == 0)
			checkError = true;
		else if (strcmp(args[i], "--frames") == 0 && i + 1 < argc)
			maxFrames = atol(args[++i]);
	}

	Application app;
	app.Start();
	if (run)
		app.SetPaused(false);
	app.SetExactForces(exact);
	if (checkError)
		app.PrintForceError();

	if (maxFrames >= 0)
		app.PrintStats();

	for (long frame = 0; maxFrames < 0 || frame < maxFrames; ++frame)
	{
		if (!app.CheckInput())
			break;
		app.PreUpdate();
		app.Update();
		app.Draw();
	}

	if (maxFrames >= 0)
		app.PrintStats();

	app.Finish();

	return 0;
}
#endif