#ifndef __MAIN__
#define __MAIN__
//#include "SDL_image\include\SDL_image.h"
//#include "SDL_mixer\include\SDL_mixer.h"

#ifdef _WIN64
#pragma comment( lib, "SDL/lib/x64/SDL2.lib" )
#pragma comment( lib, "SDL/lib/x64/SDL2main.lib" )
#else
#pragma comment( lib, "SDL/lib/x86/SDL2.lib" )
#pragma comment( lib, "SDL/lib/x86/SDL2main.lib" )
#endif
/*#pragma comment( lib, "SDL_image/libx86/SDL2_image.lib" )
#pragma comment( lib, "SDL_mixer/libx86/SDL2_mixer.lib" )*/

/*#ifdef main
#undef main
#endif*/

#include "Application.h"
#include "SDL\include\SDL.h"

float deltaTime = 0.0f;
float realDeltaTime = 0.0f;

// ----------------------------------------------------------------
int main(int argc, char *args[])
{
	Application app;
	app.Start();

	while(app.CheckInput())
	{
		app.PreUpdate();
		app.Update();
		app.Draw();
	}

	app.Finish();

	return 0;
}
#endif