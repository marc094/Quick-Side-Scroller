#include "Application.h"
#include <time.h>
#include <random>
#include <windows.h>
#include <string>
#include "PhysBody.h"
#include "Time.h"
#include "Camera.h"
#include "SDL\include\SDL.h"

extern float deltaTime;
extern float realDeltaTime;

Application::Application()
{
}


Application::~Application()
{
}


void Application::InitBody(int index)
{
	PhysBody* body = &rocks[index];

	body->mass = rand() % (MAX_MASS - MIN_MASS + 1) + MIN_MASS;
	//cel_body->diametre = rand() % (MAX_DIAMETRE - MIN_DIAMETRE + 1) + MIN_DIAMETRE;

	body->density = rand() % (MAX_DENSITY - MIN_DENSITY + 1) + MIN_DENSITY;
	body->diametre = 2 * sqrt(body->mass / (M_PI*body->density));

	body->area = (body->diametre * 0.5) * (body->diametre * 0.5) * M_PI;
	//cel_body->density = cel_body->mass / cel_body->area;

	//double r = rand() % SCREEN_WIDTH;
	scalar angle = rand() % 360;
	angle *= (2 * M_PI / 360);
	scalar distance = rand() % SPAWN_RADIUS;
	/*body->pos.x = SCREEN_WIDTH / 2;
	body->pos.y = SCREEN_HEIGHT / 2;*/
	body->pos.x = sin(angle) * distance;
	body->pos.y = cos(angle) * distance;
	body->speed.x = 0;
	body->speed.y = 0;
	/*float factor = (float)M_PI / (MAX_BODIES * 0.5f);
	cel_body->pos.x = (double)((SCREEN_WIDTH / 2) + (400) * cos(index * factor));
	cel_body->pos.y = (double)((SCREEN_HEIGHT / 2) + (400) * sin(index * factor));*/
	body->color = { 255, 255, 255 };

	body->circle.x = (int)body->pos.x;
	body->circle.y = (int)body->pos.y;
	body->circle.radius = body->diametre / 2;
	body->active = true;
}

void Application::Start()
{
	srand(time(nullptr));
	SDL_Init(SDL_INIT_EVERYTHING);

	// Create window & renderer
	window = SDL_CreateWindow("Stellar", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_BORDERLESS);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	camera = new Camera();

	// Load image lib --
	//IMG_Init(IMG_INIT_PNG);

	// Create mixer --
	/*Mix_Init(MIX_INIT_OGG);
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);*/

	rocks = (PhysBody*)malloc(MAX_BODIES * sizeof(PhysBody));

	// Init bodies --
	for (unsigned int i = 0; i < MAX_BODIES; i++)
	{
		InitBody(i);
	}

	camera->SetScale(1.0);
	camera->SetPosition({ 0.0, 0.0 });
	camera->SetTarget(nullptr);

}

void Application::Reset()
{
	for (unsigned int i = 0; i < MAX_BODIES; i++)
	{
		InitBody(i);
	}

	camera->SetPosition({ 0.0, 0.0 });
	camera->SetTarget(nullptr);
	camera->SetScale(1.0);

	reset = false;
}

// ----------------------------------------------------------------
void Application::Finish()
{
	/*Mix_CloseAudio();
	Mix_Quit();
	IMG_Quit();*/
	free(rocks);

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

// ----------------------------------------------------------------
bool Application::CheckInput()
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
			case SDLK_KP_PLUS: timescale += (scalar).1; break;
			case SDLK_KP_MINUS: timescale -= (scalar).1; break;
			case SDLK_g: G_FORCE = true; break;
			case SDLK_r: reset = true;
			case SDLK_s: camera->SetScale(0.0); break;
			case SDLK_SPACE: timescale = 0.0; break;
			case SDLK_RIGHT: camera->Move({ 10.0, 0.0 }); break;
			case SDLK_LEFT: camera->Move({ -10.0, 0.0 }); break;
			case SDLK_UP: camera->Move({ 0.0, -10.0 }); break;
			case SDLK_DOWN: camera->Move({ 0.0, 10.0 }); break;
			}
		}
		else if (event.type == SDL_KEYUP)
		{
			switch (event.key.keysym.sym)
			{
			case SDLK_g: G_FORCE = false; break;
			}
		}
		else if (event.type == SDL_QUIT)
		{
			ret = false;
		}
		else if (event.type == SDL_MOUSEBUTTONDOWN)
		{
			selectionRect.x = event.motion.x;
			selectionRect.y = event.motion.y;
			drawSelectionRect = true;
		}
		else if (event.type == SDL_MOUSEBUTTONUP)
		{
			SDL_Rect select = (selectionRect.Normalised() * (1/camera->GetScale())).toSDL();
			svec2 cam = camera->GetPosition() / camera->GetScale() - svec2(HALF_SCREEN_WIDTH, HALF_SCREEN_HEIGHT);
			select.x += (int)cam.x;
			select.y += (int)cam.y;
			if (select.w == 0) select.w++;
			if (select.h == 0) select.h++;
			camera->SetTarget(nullptr);

			std::string output = "Selection X: " + std::to_string(select.x) + "\n";
			OutputDebugString(output.c_str());
			output = "Selection Y: " + std::to_string(select.y) + "\n";
			OutputDebugString(output.c_str());
			output = "Selection W: " + std::to_string(select.w) + "\n";
			OutputDebugString(output.c_str());
			output = "Selection H: " + std::to_string(select.h) + "\n";
			OutputDebugString(output.c_str());

			scalar scale = camera->GetScale();

			for (int i = 0; i < MAX_BODIES; ++i)
			{
				if (rocks[i].active)
				{
					SDL_Rect rock = {
						(int)((rocks[i].circle.x - (int)rocks[i].circle.radius) * scale),
						(int)((rocks[i].circle.y - (int)rocks[i].circle.radius) * scale),
						(int)(rocks[i].circle.radius * 2 * scale),
						(int)(rocks[i].circle.radius * 2 * scale)
					};
					SDL_Rect result;
					if (SDL_IntersectRect(&select, &rock, &result) == SDL_TRUE)
					{
						camera->SetTarget(&rocks[i]);
						break;
					}
				}
			}

			drawSelectionRect = false;
			selectionRect = { -1,-1,0,0 };
		}
		else if (event.type == SDL_MOUSEMOTION)
		{
			if (drawSelectionRect)
			{
				selectionRect.w += event.motion.xrel;
				selectionRect.h += event.motion.yrel;
			}
		}
		else if (event.type == SDL_MOUSEWHEEL)
		{
			if (event.wheel.y > 0 && camera->GetScale() > 0.1)
			{
				camera->GetScale() /= (scalar)1.1;
			}
			else
			{
				camera->GetScale() *= (scalar)1.1;
			}
		}
	}

	return ret;
}

// ----------------------------------------------------------------
void Application::PreUpdate()
{
	realDeltaTime = frameTimeTimer.ReadSec();
	deltaTime = realDeltaTime * timescale;

	frameTimeTimer.Start();

	std::string /*output = "deltaTime: " + std::to_string(deltaTime) + "\n";
	OutputDebugString(output.c_str());*/
	output = "realDeltaTime: " + std::to_string(realDeltaTime) + "\n";
	OutputDebugString(output.c_str());

	//OutputDebugString("----------- Starting frame -----------\n");

	for (int i = 0; i < MAX_BODIES; i++)
	{
		if (rocks[i].active)
		{
			rocks[i].force.x = 0.0;
			rocks[i].force.y = 0.0;
		}
	}

	/*std::string output = "Time after force reset: " + std::to_string(frameTimeTimer.Readms()) + "\n";
	OutputDebugString(output.c_str());*/
}

// -----------------------------------------------------------------
void Application::Update()
{
	/*std::string output = "Time at Update begin(): " + std::to_string(frameTimeTimer.Readms()) + "\n";
	OutputDebugString(output.c_str());*/

	if (reset)
		Reset();

	unsigned int activeBodies = 0;

	for (int i = 0; i < MAX_BODIES; ++i)
	{
		if (rocks[i].active)
		{
			activeBodies++;
			for (int j = i + 1; j < MAX_BODIES; ++j)
			{
				if (rocks[j].active)
				{
					//Removed sqrt() and instead squared the radii sum
					bool col = (rocks[i].circle.x - rocks[j].circle.x) * (rocks[i].circle.x - rocks[j].circle.x)
						+ (rocks[i].circle.y - rocks[j].circle.y) * (rocks[i].circle.y - rocks[j].circle.y)
						< (rocks[i].circle.radius + rocks[j].circle.radius) * (rocks[i].circle.radius + rocks[j].circle.radius);

					if (col)
					{
						if (rocks[i].mass >= rocks[j].mass)
						{
							rocks[j].active = false;
							long double total_mass = (long double)(rocks[i].mass + rocks[j].mass);
							rocks[i].density = (rocks[i].density * rocks[i].mass + rocks[j].density * rocks[j].mass) / total_mass;
							rocks[i].mass = (unsigned long long)total_mass;
							rocks[i].area = rocks[i].mass / rocks[i].density;
							rocks[i].circle.radius = sqrt(rocks[i].area / M_PI);
							rocks[i].diametre = 2 * rocks[i].circle.radius;
							rocks[i].speed.x = (((rocks[i].speed.x * rocks[i].mass) + (rocks[j].speed.x * rocks[j].mass)) / (rocks[j].mass + rocks[i].mass));
							rocks[i].speed.y = (((rocks[i].speed.y * rocks[i].mass) + (rocks[j].speed.y * rocks[j].mass)) / (rocks[j].mass + rocks[i].mass));
						}
						else
						{
							rocks[i].active = false;
							long double total_mass = (long double)(rocks[i].mass + rocks[j].mass);
							rocks[j].density = (rocks[j].density * rocks[j].mass + rocks[i].density * rocks[i].mass) / total_mass;
							rocks[j].mass = (unsigned long long)total_mass;
							rocks[j].area = rocks[j].mass / rocks[j].density;
							rocks[j].circle.radius = sqrt(rocks[j].area / M_PI);
							rocks[j].diametre = 2 * rocks[j].circle.radius;
							rocks[j].speed.x = (((rocks[i].speed.x * rocks[i].mass) + (rocks[j].speed.x * rocks[j].mass)) / (rocks[j].mass + rocks[i].mass));
							rocks[j].speed.y = (((rocks[i].speed.y * rocks[i].mass) + (rocks[j].speed.y * rocks[j].mass)) / (rocks[j].mass + rocks[i].mass));
						}
					}
					else
					{
						svec2 distance = rocks[j].pos - rocks[i].pos;

						double gForce = ((G_CONSTANT * rocks[j].mass * rocks[i].mass) / max(distance.sqrLength(), 1.0));

						distance.normalise();

						rocks[i].force += distance * gForce;
						rocks[j].force -= distance * gForce;

					}
				}
			}

			if (G_FORCE)
			{
				svec2 screenCentre = svec2(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
				svec2 delta = rocks[i].pos - screenCentre;
				delta.normalise();
				rocks[i].speed -= delta * ((100000000000. / rocks[i].mass) * deltaTime);
			}

			svec2 acceleration = (rocks[i].force / rocks[i].mass);
			rocks[i].speed += acceleration * deltaTime;

			rocks[i].pos += (rocks[i].speed * deltaTime);

			//Friction (buggy)
			//rocks[i].speed = rocks[i].speed - (rocks[i].speed * (.00000000001 * timescale));

			rocks[i].circle.x = (int)rocks[i].pos.x;
			rocks[i].circle.y = (int)rocks[i].pos.y;
		}
	}

	totalActiveBodies = activeBodies;

	camera->Update();

	/*output = "Time at Update() end: " + std::to_string(frameTimeTimer.Readms()) + "\n";
	OutputDebugString(output.c_str());*/
}

// ----------------------------------------------------------------
void Application::Draw()
{
	/*std::string output = "Time at Draw() begin: " + std::to_string(frameTimeTimer.Readms()) + "\n";
	OutputDebugString(output.c_str());*/

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	SDL_Point points[CIRCLE_POINTS];
	uint point_number = 0;
	scalar factor = 1.f;
	scalar cameraScale = camera->GetScale();
	svec2 cameraPos = camera->GetPosition();

	for (int i = 0; i < MAX_BODIES; ++i)
	{
		if (!rocks[i].active)
			continue;

		//SDL_SetRenderDrawColor(renderer, rocks[i].color.r, rocks[i].color.g, rocks[i].color.b, 255);

		if (rocks[i].diametre * cameraScale > 1)
		{
			point_number = min((uint)(rocks[i].diametre * cameraScale) + 1, (uint)CIRCLE_POINTS);

			factor = (float)M_PI / (point_number / 2.f);

			for (unsigned int j = 0; j < point_number; ++j)
			{
				points[j].x = ((rocks[i].pos.x + (rocks[i].diametre / 2) * cos(j * factor)) - cameraPos.x) * cameraScale + HALF_SCREEN_WIDTH;
				points[j].y = ((rocks[i].pos.y + (rocks[i].diametre / 2) * sin(j * factor)) - cameraPos.y) * cameraScale + HALF_SCREEN_HEIGHT;
			}
			SDL_RenderDrawPoints(renderer, points, point_number);
		}
		else
		{
			SDL_RenderDrawPoint(renderer, (rocks[i].pos.x - cameraPos.x) * cameraScale + HALF_SCREEN_WIDTH, (rocks[i].pos.y - cameraPos.y) * cameraScale + HALF_SCREEN_HEIGHT);
		}
	}

	PhysBody* cameraTarget = camera->GetTarget();

	if (cameraTarget != nullptr)
	{
		svec2 targetPosRelative = (cameraTarget->pos - cameraPos) * cameraScale + svec2(HALF_SCREEN_WIDTH, HALF_SCREEN_HEIGHT);
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
		SDL_RenderDrawLine(renderer, targetPosRelative.x, targetPosRelative.y, targetPosRelative.x + (cameraTarget->force.x / cameraTarget->mass) * FORCE_FACTOR_DRAW, targetPosRelative.y + (cameraTarget->force.y / cameraTarget->mass) * FORCE_FACTOR_DRAW);
		SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
		SDL_RenderDrawLine(renderer, targetPosRelative.x, targetPosRelative.y, targetPosRelative.x + camera->GetTarget()->speed.x, targetPosRelative.y + camera->GetTarget()->speed.y);
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

	}

	if (drawSelectionRect)
	{
		SDL_RenderDrawRect(renderer, &selectionRect.toSDL());
	}

	// Finally swap buffers
	SDL_RenderPresent(renderer);

	/*output = "Time at Draw() end: " + std::to_string(frameTimeTimer.Readms()) + "\n";
	OutputDebugString(output.c_str());*/
}
