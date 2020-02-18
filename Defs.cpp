#include "Defs.h"
#include <time.h>
#include <random>
#include "Timer.h"
#include <windows.h>
#include <string>

struct globals
{
	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;
	CelestialBody rocks[MAX_BODIES];
	bool G_FORCE = false;
	double timescale = INITIAL_TIME_SCALE;
	bool reset = false;
	dRect camera = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
	double cameraSpeed = 0;
	CelestialBody* trackingBody = nullptr;
	iRect selectionRect = { 0,0,0,0 };
	bool drawSelectionRect = false;
	unsigned int totalActiveBodies = 0u;
	Timer frameTimeTimer;
	float deltaTime = 0.f;
	float realDeltaTime = 0.f;
	double scale = 1.0;
} g; // automatically create an insteance called "g"

bool CircleInCircle(Circle c1, Circle c2)
{
	if (sqrt(pow(c1.x - c2.x, 2) + pow(c1.y - c2.y, 2)) < c1.radius + c2.radius)
		return true;
	return false;
}

dvec2 Interpolate(dvec2 value, dvec2 target, double step)
{
	dvec2 totalDisplacement = target - value;
	dvec2 displacement = totalDisplacement.normalised() * step;
	dvec2 finalVector;
	if (displacement.length() < totalDisplacement.length())
	{
		finalVector = value + displacement;
	}
	else finalVector = target;
	return finalVector;
}

bool CheckCollision(CelestialBody b1, CelestialBody b2)
{
	if (CircleInCircle(b1.circle, b2.circle))
		return true;
	return false;
}

void InitBody(int index) {
	CelestialBody* cel_body = &g.rocks[index];

	cel_body->mass = rand() % (MAX_MASS - MIN_MASS + 1) + MIN_MASS;
	//cel_body->diametre = rand() % (MAX_DIAMETRE - MIN_DIAMETRE + 1) + MIN_DIAMETRE;
	//double pi = M_PI;

	cel_body->density = rand() % (MAX_DENSITY - MIN_DENSITY + 1) + MIN_DENSITY;
	cel_body->diametre = 2 * sqrt(cel_body->mass / (M_PI*cel_body->density));

	cel_body->area = (cel_body->diametre * 0.5) * (cel_body->diametre * 0.5) * M_PI;
	//cel_body->density = cel_body->mass / cel_body->area;

	//double r = rand() % SCREEN_WIDTH;
	dvec2 spawnZone = dvec2(SPAWN_SQUARE_DIMS, SPAWN_SQUARE_DIMS);
	cel_body->pos.x = SCREEN_WIDTH / 2;
	cel_body->pos.y = SCREEN_HEIGHT / 2;
	cel_body->pos.x += rand() % (int)spawnZone.x - (int)(spawnZone.x / 2);
	cel_body->pos.y += rand() % (int)spawnZone.y - (int)(spawnZone.y / 2);
	cel_body->speed.x = 0;
	cel_body->speed.y = 0;
	/*float factor = (float)M_PI / (MAX_BODIES * 0.5f);
	cel_body->pos.x = (double)((SCREEN_WIDTH / 2) + (400) * cos(index * factor));
	cel_body->pos.y = (double)((SCREEN_HEIGHT / 2) + (400) * sin(index * factor));*/
	cel_body->color = { 255, 255, 255 };

	cel_body->circle.x = (int)cel_body->pos.x;
	cel_body->circle.y = (int)cel_body->pos.y;
	cel_body->circle.radius = cel_body->diametre / 2;
	cel_body->active = true;
}

void Start()
{
	srand(time(nullptr));
	SDL_Init(SDL_INIT_EVERYTHING);

	// Create window & renderer
	g.window = SDL_CreateWindow("Stellar", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_BORDERLESS);
	g.renderer = SDL_CreateRenderer(g.window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	// Load image lib --
	//IMG_Init(IMG_INIT_PNG);

	// Create mixer --
	/*Mix_Init(MIX_INIT_OGG);
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);*/

	// Init bodies --
	for (unsigned int i = 0; i < MAX_BODIES; i++) {
		InitBody(i);
	}
	g.camera.x = 0;
	g.camera.y = 0;
	g.trackingBody = nullptr;

}

void Reset()
{
	for (unsigned int i = 0; i < MAX_BODIES; i++)
	{
		InitBody(i);
	}

	g.camera.x = 0;
	g.camera.y = 0;
	g.trackingBody = nullptr;

	g.reset = false;
}

// ----------------------------------------------------------------
void Finish()
{
	/*Mix_CloseAudio();
	Mix_Quit();
	IMG_Quit();*/
	SDL_DestroyRenderer(g.renderer);
	SDL_DestroyWindow(g.window);
	SDL_Quit();
}

// ----------------------------------------------------------------
bool CheckInput()
{
	bool ret = true;
	SDL_Event event;

	while (SDL_PollEvent(&event) != 0)
	{
		if (event.type == SDL_KEYDOWN)
		{
			switch (event.key.keysym.sym)
			{
			case SDLK_ESCAPE: ret = false; break;
			case SDLK_KP_PLUS: g.timescale += .1; break;
			case SDLK_KP_MINUS: g.timescale -= .1; break;
			case SDLK_g: g.G_FORCE = true; break;
			case SDLK_r: g.reset = true;
			case SDLK_RIGHT: g.camera.x+=10; break;
			case SDLK_LEFT: g.camera.x-=10; break;
			case SDLK_UP: g.camera.y-=10; break;
			case SDLK_DOWN: g.camera.y+=10; break;
			}
		}
		else if (event.type == SDL_KEYUP)
		{
			switch (event.key.keysym.sym)
			{
			case SDLK_g: g.G_FORCE = false; break;
			}
		}
		else if (event.type == SDL_QUIT)
		{
			ret = false;
		}
		else if (event.type == SDL_MOUSEBUTTONDOWN)
		{
			g.selectionRect.x = event.motion.x;
			g.selectionRect.y = event.motion.y;
			g.drawSelectionRect = true;
		}
		else if (event.type == SDL_MOUSEBUTTONUP)
		{
			SDL_Rect select = g.selectionRect.Normalised().toSDL();
			select.x += (int)g.camera.x;
			select.y += (int)g.camera.y;
			if (select.w == 0) select.w++;
			if (select.h == 0) select.h++;
			g.trackingBody = nullptr;

			for (int i = 0; i < MAX_BODIES; ++i)
			{
				if (g.rocks[i].active)
				{
					SDL_Rect rock = { g.rocks[i].circle.x - (int)g.rocks[i].circle.radius, g.rocks[i].circle.y - (int)g.rocks[i].circle.radius , (int)(g.rocks[i].circle.radius * 2) , (int)(g.rocks[i].circle.radius * 2)};
					SDL_Rect result;
					if (SDL_IntersectRect(&select, &rock, &result) == SDL_TRUE)
					{
						g.trackingBody = &g.rocks[i];
					}
				}
			}
			g.drawSelectionRect = false;
			g.selectionRect = { -1,-1,0,0 };
		}
		else if (event.type == SDL_MOUSEMOTION)
		{
			if (g.drawSelectionRect) {
				g.selectionRect.w += event.motion.xrel;
				g.selectionRect.h += event.motion.yrel;
			}
		}
	}

	return ret;
}

// ----------------------------------------------------------------
void PreUpdate()
{
	g.realDeltaTime = g.frameTimeTimer.ReadSec();
	g.deltaTime = g.realDeltaTime * g.timescale;

	g.frameTimeTimer.Start();

	OutputDebugString("----------- Starting frame -----------\n");

	for (int i = 0; i < MAX_BODIES; i++)
	{
		if (g.rocks[i].active)
		{
			g.rocks[i].force.x = 0.0;
			g.rocks[i].force.y = 0.0;
		}
	}

	std::string output = "Time after force reset: " + std::to_string(g.frameTimeTimer.Readms()) + "\n";
	OutputDebugString(output.c_str());
}

void Update()
{
	std::string output = "Time at Update begin(): " + std::to_string(g.frameTimeTimer.Readms()) + "\n";
	OutputDebugString(output.c_str());

	if (g.reset)
		Reset();

	unsigned int activeBodies = 0;

	for (int i = 0; i < MAX_BODIES; ++i)
	{
		if (g.rocks[i].active)
		{
			activeBodies++;
			for (int j = i + 1; j < MAX_BODIES; ++j)
			{
				if (g.rocks[j].active)
				{
					//Removed sqrt() and instead squared the radii sum
					bool col = (g.rocks[i].circle.x - g.rocks[j].circle.x) * (g.rocks[i].circle.x - g.rocks[j].circle.x)
						+ (g.rocks[i].circle.y - g.rocks[j].circle.y) * (g.rocks[i].circle.y - g.rocks[j].circle.y)
						< (g.rocks[i].circle.radius + g.rocks[j].circle.radius) * (g.rocks[i].circle.radius + g.rocks[j].circle.radius)/* * 0.8*/;

					if (col) {
						if (g.rocks[i].mass >= g.rocks[j].mass)
						{
							g.rocks[j].active = false;
							long double total_mass = (long double)(g.rocks[i].mass + g.rocks[j].mass);
							g.rocks[i].density = (g.rocks[i].density * g.rocks[i].mass + g.rocks[j].density * g.rocks[j].mass) / total_mass;
							g.rocks[i].mass = (unsigned long long)total_mass;
							g.rocks[i].area = g.rocks[i].mass / g.rocks[i].density;
							g.rocks[i].circle.radius = sqrt(g.rocks[i].area / M_PI);
							g.rocks[i].diametre = 2 * g.rocks[i].circle.radius;
							g.rocks[i].speed.x = (((g.rocks[i].speed.x * g.rocks[i].mass) + (g.rocks[j].speed.x * g.rocks[j].mass)) / (g.rocks[j].mass + g.rocks[i].mass));
							g.rocks[i].speed.y = (((g.rocks[i].speed.y * g.rocks[i].mass) + (g.rocks[j].speed.y * g.rocks[j].mass)) / (g.rocks[j].mass + g.rocks[i].mass));
						}
						else
						{
							g.rocks[i].active = false;
							long double total_mass = (long double)(g.rocks[i].mass + g.rocks[j].mass);
							g.rocks[j].density = (g.rocks[j].density * g.rocks[j].mass + g.rocks[i].density * g.rocks[i].mass) / total_mass;
							g.rocks[j].mass = (unsigned long long)total_mass;
							g.rocks[j].area = g.rocks[j].mass / g.rocks[j].density;
							g.rocks[j].circle.radius = sqrt(g.rocks[j].area / M_PI);
							g.rocks[j].diametre = 2 * g.rocks[j].circle.radius;
							g.rocks[j].speed.x = (((g.rocks[i].speed.x * g.rocks[i].mass) + (g.rocks[j].speed.x * g.rocks[j].mass)) / (g.rocks[j].mass + g.rocks[i].mass));
							g.rocks[j].speed.y = (((g.rocks[i].speed.y * g.rocks[i].mass) + (g.rocks[j].speed.y * g.rocks[j].mass)) / (g.rocks[j].mass + g.rocks[i].mass));
						}
					}
					else
					{
						dvec2 distance = g.rocks[j].pos - g.rocks[i].pos;
						double gForce = ((G_CONSTANT * g.rocks[j].mass * g.rocks[i].mass) / distance.sqrLength());

						distance.normalise();

						g.rocks[i].force += distance * gForce;
						g.rocks[j].force -= distance * gForce;
					}
				}
			}

			if (g.G_FORCE)
			{
				dvec2 screenCentre = dvec2(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
				dvec2 delta = g.rocks[i].pos - screenCentre;
				delta.normalise();
				g.rocks[i].speed -= delta * ((100000000000. / g.rocks[i].mass) * g.deltaTime);
			}

			dvec2 acceleration = (g.rocks[i].force / g.rocks[i].mass);
			g.rocks[i].speed += acceleration * g.deltaTime;

			g.rocks[i].pos += (g.rocks[i].speed * g.deltaTime);

			//Friction (buggy)
			//g.rocks[i].speed = g.rocks[i].speed - (g.rocks[i].speed * (.00000000001 * g.timescale));

			g.rocks[i].circle.x = (int)g.rocks[i].pos.x;
			g.rocks[i].circle.y = (int)g.rocks[i].pos.y;
		}
	}

	g.totalActiveBodies = activeBodies;

	if (g.trackingBody != nullptr)
	{
		g.cameraSpeed = Interpolate(g.cameraSpeed, CAMERA_MOVEMENT_SPEED, 0.1);
		dvec2 cameraTarget = dvec2(g.trackingBody->pos.x - g.camera.w / 2, g.trackingBody->pos.y - g.camera.h / 2);
		dvec2 cameraPos = dvec2(g.camera.x, g.camera.y);
		cameraPos = Interpolate(cameraPos, cameraTarget, g.cameraSpeed);
		g.camera.x = cameraPos.x;
		g.camera.y = cameraPos.y;

		if (!g.trackingBody->active)
			g.trackingBody = nullptr;
	}
	else g.cameraSpeed = 0;

	output = "Time at Update() end: " + std::to_string(g.frameTimeTimer.Readms()) + "\n";
	OutputDebugString(output.c_str());
}

// ----------------------------------------------------------------
void Draw()
{
	std::string output = "Time at Draw() begin: " + std::to_string(g.frameTimeTimer.Readms()) + "\n";
	OutputDebugString(output.c_str());

	SDL_SetRenderDrawColor(g.renderer, 0, 0, 0, 255);
	SDL_RenderClear(g.renderer);

	SDL_SetRenderDrawColor(g.renderer, 255, 255, 255, 255);
	SDL_SetRenderDrawBlendMode(g.renderer, SDL_BLENDMODE_BLEND);

	for (int i = 0; i < MAX_BODIES; ++i)
	{
		if (!g.rocks[i].active)
			continue;

		//SDL_SetRenderDrawColor(g.renderer, g.rocks[i].color.r, g.rocks[i].color.g, g.rocks[i].color.b, 255);

		if (g.rocks[i].diametre > 1) {
			int result = -1;
			uint point_number = min((uint)g.rocks[i].diametre + 1, (uint)CIRCLE_POINTS);
			SDL_Point points[CIRCLE_POINTS];

			float factor = (float)M_PI / (point_number / 2.f);

			for (unsigned int j = 0; j < point_number; ++j)
			{
				points[j].x = (int)(g.rocks[i].pos.x + (g.rocks[i].diametre / 2) * cos(j * factor)) - g.camera.x;
				points[j].y = (int)(g.rocks[i].pos.y + (g.rocks[i].diametre / 2) * sin(j * factor)) - g.camera.y;
			}

			result = SDL_RenderDrawPoints(g.renderer, points, point_number);
		}
		else {
			SDL_Point p = { g.rocks[i].pos.x - g.camera.x, g.rocks[i].pos.y - g.camera.y };
			SDL_RenderDrawPoints(g.renderer, &p, 1);
		}
	}

	if (g.trackingBody != nullptr)
	{
		dvec2 cameraTarget = dvec2(g.trackingBody->pos.x - g.camera.x, g.trackingBody->pos.y - g.camera.y);
		SDL_SetRenderDrawColor(g.renderer, 255, 0, 0, 255);
		SDL_RenderDrawLine(g.renderer, cameraTarget.x, cameraTarget.y, cameraTarget.x + (g.trackingBody->force.x/g.trackingBody->mass) * 40, cameraTarget.y + (g.trackingBody->force.y/g.trackingBody->mass) * 40);
		SDL_SetRenderDrawColor(g.renderer, 0, 255, 0, 255);
		SDL_RenderDrawLine(g.renderer, cameraTarget.x, cameraTarget.y, cameraTarget.x + g.trackingBody->speed.x, cameraTarget.y + g.trackingBody->speed.y);
		SDL_SetRenderDrawColor(g.renderer, 255, 255, 255, 255);
	}

	if (g.drawSelectionRect)
	{
		SDL_RenderDrawRect(g.renderer, &g.selectionRect.toSDL());
	}

	// Finally swap buffers
	SDL_RenderPresent(g.renderer);

	output = "Time at Draw() end: " + std::to_string(g.frameTimeTimer.Readms()) + "\n";
	OutputDebugString(output.c_str());
}
