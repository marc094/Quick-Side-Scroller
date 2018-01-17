#ifndef __MAIN__
#define __MAIN__
//#include "SDL_image\include\SDL_image.h"
//#include "SDL_mixer\include\SDL_mixer.h"

#include "Defs.h"

#pragma comment( lib, "SDL/libx86/SDL2.lib" )
#pragma comment( lib, "SDL/libx86/SDL2main.lib" )
/*#pragma comment( lib, "SDL_image/libx86/SDL2_image.lib" )
#pragma comment( lib, "SDL_mixer/libx86/SDL2_mixer.lib" )*/

/*#ifdef main
#undef main
#endif*/

// ----------------------------------------------------------------
int main(int argc, char *args[])
{
	Start();

	while(CheckInput())
	{
		Update();
		Draw();
	}

	Finish();

	return 0;
}
#endif