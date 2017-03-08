// ----------------------------------------------------------------
// Quick Side Scroller (https://github.com/d0n3val/Quick-Side-Scroller)
// Simplistic side scroller made with SDL for educational purposes.
//
// Installation
// Project files are made for VS 2015. Download the code, compile it. There is no formal installation process.
//
// Credits
// Ricard Pillosu
//
// License
// This is free and unencumbered software released into the public domain.
//
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled
// binary, for any purpose, commercial or non - commercial, and by any
// means.
//
// In jurisdictions that recognize copyright laws, the author or authors
// of this software dedicate any and all copyright interest in the
// software to the public domain.We make this dedication for the benefit
// of the public at large and to the detriment of our heirs and
// successors.We intend this dedication to be an overt act of
// relinquishment in perpetuity of all present and future rights to this
// software under copyright law.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
// OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
//
// For more information, please refer to <http://unlicense.org/>
// 
// ----------------------------------------------------------------

#include "SDL\include\SDL.h"
#include "SDL_image\include\SDL_image.h"
#include "SDL_mixer\include\SDL_mixer.h"
#include <math.h>

#pragma comment( lib, "SDL/libx86/SDL2.lib" )
#pragma comment( lib, "SDL/libx86/SDL2main.lib" )
#pragma comment( lib, "SDL_image/libx86/SDL2_image.lib" )
#pragma comment( lib, "SDL_mixer/libx86/SDL2_mixer.lib" )

// Globals --------------------------------------------------------
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define SCROLL_SPEED 5
#define SHIP_SPEED 3
#define NUM_SHOTS 32
#define SHOT_SPEED 5
#define MAX_ROCKS 16


struct rectangle {
	int x, y, w, h;
};

struct circle {
	int x, y;
	int radius;
};

struct projectile
{
	rectangle rect{ 0, 0, 35, 32 };
	bool alive;
};

struct rock {
	circle circl;
	bool alive;
	int rotation;
	float speed;
	int angle = 0;
	int stage = 0;
};

struct globals
{
	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;
	SDL_Texture* background = nullptr;
	SDL_Texture* ship = nullptr;
	SDL_Texture* shot = nullptr;
	SDL_Texture* asteroid = nullptr;
	int background_width = 0;
	int ship_x = 0;
	int ship_y = 0;
	int last_shot = 0;
	bool fire, up, down, left, right;
	Mix_Music* music = nullptr;
	Mix_Chunk* fx_shoot = nullptr;
	int scroll = 0;
	projectile shots[NUM_SHOTS];
	rock rocks[MAX_ROCKS];
} g; // automatically create an insteance called "g"

// ----------------------------------------------------------------
bool pointInRectangle(int x, int y, rectangle rect) {
	if (x < rect.w + rect.x && x > rect.x)
	{
		if (y < rect.h + rect.y && y > rect.y)
			return true;
	}
	return false;
}

bool squareInCirlce(circle circl, rectangle rect) {
	for (int i = 0; i < 4; i++) {
		switch (i) {
		case 0:
			if (sqrt(pow(rect.x - circl.x, 2) + pow(rect.y - circl.y, 2)) <= circl.radius) {
				return true;
			}
			break;
		case 1:
			if (sqrt(pow(rect.x + rect.w - circl.x, 2) + pow(rect.y - circl.y, 2)) <= circl.radius) {
				return true;
			}
			break;
		case 2:
			if (sqrt(pow(rect.x + rect.w - circl.x, 2) + pow(rect.y + rect.h - circl.y, 2)) <= circl.radius) {
				return true;
			}
			break;
		case 3:
			if (sqrt(pow(rect.x - circl.x, 2) + pow(rect.y + rect.h - circl.y, 2)) <= circl.radius) {
				return true;
			}
			break;
		}
	}
	return false;
}

bool checkCollision(rock rock, projectile proj)
{
	if (pointInRectangle(rock.circl.x, rock.circl.y, proj.rect))
		return true;
	if (squareInCirlce(rock.circl, proj.rect))
		return true;
	return false;
}

void initNextRock(int index) {
	rock* prev_rock = &g.rocks[index];

	if (prev_rock->circl.radius >= 25) {
		rock* new_rock = nullptr;
		for (int i = index; i < MAX_ROCKS; i++) {
			if (!g.rocks[i].alive) {
				new_rock = &g.rocks[i];
				break;
			}
		}
		new_rock->circl.radius = prev_rock->circl.radius * 0.75;
		prev_rock->circl.radius = new_rock->circl.radius;
		new_rock->circl.x = prev_rock->circl.x;
		new_rock->circl.y = prev_rock->circl.y + prev_rock->circl.radius;
		prev_rock->circl.y = prev_rock->circl.y - prev_rock->circl.radius;
		new_rock->alive = true;
		prev_rock->rotation *= 2;
		new_rock->rotation = prev_rock->rotation;
		prev_rock->stage += 1;
		new_rock->stage = prev_rock->stage;
		prev_rock->speed = -5 + prev_rock->stage;
		new_rock->speed = -5 + new_rock->stage;
	}
	else {
		prev_rock->alive = false;
	}
}

void Start()
{
	SDL_Init(SDL_INIT_EVERYTHING);

	// Create window & renderer
	g.window = SDL_CreateWindow("QSS - Quick Side Scroller - 0.5", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
	g.renderer = SDL_CreateRenderer(g.window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	// Load image lib --
	IMG_Init(IMG_INIT_PNG);
	g.background = SDL_CreateTextureFromSurface(g.renderer, IMG_Load("assets/background.png"));
	g.ship = SDL_CreateTextureFromSurface(g.renderer, IMG_Load("assets/ship.png"));
	g.shot = SDL_CreateTextureFromSurface(g.renderer, IMG_Load("assets/shot.png"));
	g.asteroid = SDL_CreateTextureFromSurface(g.renderer, IMG_Load("assets/rock.png"));
	SDL_QueryTexture(g.background, nullptr, nullptr, &g.background_width, nullptr);

	// Create mixer --
	Mix_Init(MIX_INIT_OGG);
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
	g.music = Mix_LoadMUS("assets/music.ogg");
	Mix_PlayMusic(g.music, -1);
	g.fx_shoot = Mix_LoadWAV("assets/laser.wav");

	// Init other vars --
	g.ship_x = 100;
	g.ship_y = SCREEN_HEIGHT / 2;
	g.fire = g.up = g.down = g.left = g.right = false;

	// Init rocks --
	g.rocks[0].alive = true;
	g.rocks[0].circl.x = SCREEN_WIDTH;
	g.rocks[0].circl.y = SCREEN_HEIGHT / 2;
	g.rocks[0].speed = 1;
	g.rocks[0].circl.radius = 75;
	g.rocks[0].rotation = 1;
			
}

// ----------------------------------------------------------------
void Finish()
{
	Mix_FreeMusic(g.music);
	Mix_FreeChunk(g.fx_shoot);
	Mix_CloseAudio();
	Mix_Quit();
	SDL_DestroyTexture(g.shot);
	SDL_DestroyTexture(g.ship);
	SDL_DestroyTexture(g.background);
	IMG_Quit();
	SDL_DestroyRenderer(g.renderer);
	SDL_DestroyWindow(g.window);
	SDL_Quit();
}

// ----------------------------------------------------------------
bool CheckInput()
{
	bool ret = true;
	SDL_Event event;

	while(SDL_PollEvent(&event) != 0)
	{
		if(event.type == SDL_KEYUP)
		{
			switch(event.key.keysym.sym)
			{
				case SDLK_UP: g.up = false;	break;
				case SDLK_DOWN:	g.down = false;	break;
				case SDLK_LEFT:	g.left = false;	break;
				case SDLK_RIGHT: g.right = false; break;
			}
		}
		else if(event.type == SDL_KEYDOWN)
		{
			switch(event.key.keysym.sym)
			{
				case SDLK_UP: g.up = true; break;
				case SDLK_DOWN: g.down = true; break;
				case SDLK_LEFT: g.left = true; break;
				case SDLK_RIGHT: g.right = true; break;
				case SDLK_ESCAPE: ret = false; break;
				case SDLK_SPACE: g.fire = (event.key.repeat == 0); break;
			}
		}
		else if (event.type == SDL_QUIT)
			ret = false;
	}

	return ret;
}

// ----------------------------------------------------------------
void MoveStuff()
{
	// Calc new ship position
	if(g.up) g.ship_y -= SHIP_SPEED;
	if(g.down) g.ship_y += SHIP_SPEED;
	if(g.left) g.ship_x -= SHIP_SPEED;
	if(g.right)	g.ship_x += SHIP_SPEED;

	if(g.fire)
	{
		Mix_PlayChannel(-1, g.fx_shoot, 0);
		g.fire = false;

		if(g.last_shot == NUM_SHOTS)
			g.last_shot = 0;

		g.shots[g.last_shot].alive = true;
		g.shots[g.last_shot].rect.x = g.ship_x + 32;
		g.shots[g.last_shot].rect.y = g.ship_y + 32 - g.shots[g.last_shot].rect.h/2;
		++g.last_shot;
	}

	for(int i = 0; i < NUM_SHOTS; ++i)
	{
		if(g.shots[i].alive)
		{
			if(g.shots[i].rect.x < SCREEN_WIDTH)
				g.shots[i].rect.x += SHOT_SPEED;
			else
				g.shots[i].alive = false;

			for (int j = 0; j < MAX_ROCKS; j++) {
				if (g.rocks[j].alive) {
					if (checkCollision(g.rocks[j], g.shots[i])) {
						g.shots[i].alive = false;
						initNextRock(j);
					}
				}
			}
		}
	}

	for (int i = 0; i < MAX_ROCKS; ++i)
	{
		if (g.rocks[i].alive)
		{
			if (g.rocks[i].speed < g.rocks[i].stage * 2)
				g.rocks[i].speed += 0.2;
			g.rocks[i].circl.x -= g.rocks[i].speed;
			g.rocks[i].angle += g.rocks[i].rotation;
			if (g.rocks[i].circl.x + g.rocks[i].circl.radius < 0)
				g.rocks[i].circl.x = SCREEN_WIDTH + g.rocks[i].circl.radius;
		}
	}
}

// ----------------------------------------------------------------
void Draw()
{
	SDL_Rect target;

	// Scroll and draw background
	g.scroll += SCROLL_SPEED;
	if(g.scroll >= g.background_width)
		g.scroll = 0;

	target = { -g.scroll, 0, g.background_width, SCREEN_HEIGHT };
	
	SDL_RenderCopy(g.renderer, g.background, nullptr, &target);
	target.x += g.background_width;
	SDL_RenderCopy(g.renderer, g.background, nullptr, &target);

	// Draw player's ship --
	target = { g.ship_x, g.ship_y, 64, 64 };
	SDL_RenderCopy(g.renderer, g.ship, nullptr, &target);

	// Draw lasers --
	for(int i = 0; i < NUM_SHOTS; ++i)
	{
		if(g.shots[i].alive)
		{
			target = { g.shots[i].rect.x, g.shots[i].rect.y, g.shots[i].rect.w, g.shots[i].rect.h };
			SDL_RenderCopy(g.renderer, g.shot, nullptr, &target);
		}
	}

	// Draw rocks --
	for (int i = 0; i < MAX_ROCKS; ++i)
	{
		if (g.rocks[i].alive)
		{
			target = { g.rocks[i].circl.x - g.rocks[i].circl.radius, g.rocks[i].circl.y - g.rocks[i].circl.radius, g.rocks[i].circl.radius*2, g.rocks[i].circl.radius*2 };
			SDL_RenderCopyEx(g.renderer, g.asteroid, nullptr, &target, g.rocks[i].angle, nullptr, SDL_FLIP_NONE);
		}
	}

	// Finally swap buffers
	SDL_RenderPresent(g.renderer);
}

// ----------------------------------------------------------------
int main(int argc, char* args[])
{
	Start();

	while(CheckInput())
	{
		MoveStuff();
		Draw();
	}

	Finish();

	return(0); // EXIT_SUCCESS
}