#include "Application.h"
#include <time.h>
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
	rng.seed((unsigned long long)time(null));
}


Application::~Application()
{
}


void Application::InitBody(PhysBody* body)
{
	static int index = 0;
	std::uniform_int_distribution<std::mt19937_64::result_type> distributionMass(MIN_MASS, MAX_MASS);
	std::uniform_int_distribution<std::mt19937_64::result_type> distributionDensity(MIN_DENSITY, MAX_DENSITY);

	body->mass = distributionMass(rng);
	//cel_body->diametre = rand() % (MAX_DIAMETRE - MIN_DIAMETRE + 1) + MIN_DIAMETRE;

	body->density = distributionDensity(rng);
	body->diametre = 2 * sqrt(body->mass / (M_PI*body->density));

	body->area = (body->diametre * 0.5) * (body->diametre * 0.5) * M_PI;
	//cel_body->density = cel_body->mass / cel_body->area;

	body->pos = GenerateInitialPosition();
	/*scalar speed = (rand() % (int)(STARTING_SPEED_MAGNITUDE * 0.1) + STARTING_SPEED_MAGNITUDE) * sqrt(distance) / body->mass;
	body->speed.x = speed * sin(angle + M_PI / 2);
	body->speed.y = speed * cos(angle + M_PI / 2);*/
	/*float factor = (float)M_PI / (MAX_BODIES * 0.5f);
	cel_body->pos.x = (double)((SCREEN_WIDTH / 2) + (400) * cos(index * factor));
	cel_body->pos.y = (double)((SCREEN_HEIGHT / 2) + (400) * sin(index * factor));*/
	body->speed = sZero;
	body->force = sZero;

	body->color = { 255, 255, 255 };

	body->circle.x = (int)body->pos.x;
	body->circle.y = (int)body->pos.y;
	body->circle.radius = body->diametre / 2;
	body->active = true;
	body->trailIndex = 0;
	body->trailRecordIndex = index++ % TRAIL_UPDATE_FREQUENCY;

	for (int i = 0; i < TRAIL_LENGTH; i++)
	{
		body->trail[i] = body->pos;
	}
}

void Application::Start()
{
	srand(time(nullptr));
	SDL_Init(SDL_INIT_EVERYTHING);

	// Create window & renderer
	window = SDL_CreateWindow("Stellar", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_BORDERLESS);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	camera = new Camera();

	// Load image lib --
	//IMG_Init(IMG_INIT_PNG);

	// Create mixer --
	/*Mix_Init(MIX_INIT_OGG);
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);*/

	rocks = ARR_DECL(PhysBody, MAX_BODIES);
	//rocks = std::list<PhysBody*>();
	/*for (int i = 0; i < MAX_BODIES; i++)
		rocks.push_back(new PhysBody());*/

	// Init bodies --
	/*for (std::list<PhysBody*>::iterator it = rocks.begin(); it != rocks.end(); it++)
	{
		InitBody(*it);
	}*/
	for (int i = 0; i < MAX_BODIES; i++)
	{
		InitBody(&rocks[i]);
	}

	camera->SetScale(1.0);
	camera->SetPosition({ 0.0, 0.0 });
	camera->SetTarget(nullptr);

}

void Application::Reset()
{
	/*for (std::list<PhysBody*>::iterator it = rocks.begin(); it != rocks.end(); it++)
	{
		delete[] *it;
	}
	rocks.clear();

	for (int i = 0; i < MAX_BODIES; i++)
		rocks.push_back(new PhysBody());

	for (std::list<PhysBody*>::iterator it = rocks.begin(); it != rocks.end(); it++)
	{
		InitBody(*it);
	}*/

	for (int i = 0; i < MAX_BODIES; i++)
	{
		InitBody(&rocks[i]);
	}

	camera->SetPosition({ 0.0, 0.0 });
	camera->SetTarget(nullptr);
	camera->SetScale(1.0);

	paused = true;

	reset = false;
}

// ----------------------------------------------------------------
void Application::Finish()
{
	/*Mix_CloseAudio();
	Mix_Quit();
	IMG_Quit();*/
	ARR_FREE(rocks);
	/*for (std::list<PhysBody*>::iterator it = rocks.begin(); it != rocks.end(); it++)
	{
		delete *it;
	}
	rocks.clear();*/

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

svec2 Application::GenerateInitialPosition()
{
	svec2 position;
	switch (INITIAL_DISTRIBUTION)
	{
	case InitialDistribution::RADIAL_CENTRE_DENSER:
	{
		scalar angle = rand() % 360;
		angle *= (2 * M_PI / 360);
		scalar distance = rand() % SPAWN_RADIUS;
		position.x = sin(angle) * distance;
		position.y = cos(angle) * distance;
		break;
	}
	case InitialDistribution::RADIAL_UNIFORM:
	{
		bool correct = false;
		while (!correct)
		{
			position.x = rand() % (2 * SPAWN_RADIUS) - SPAWN_RADIUS;
			position.y = rand() % (2 * SPAWN_RADIUS) - SPAWN_RADIUS;

			if (svec2::sDistance(sZero, position) < SPAWN_RADIUS)
				correct = true;
		}
		break;
	}
	case InitialDistribution::SQUARE_CENTRE_DENSER:
	{
		/*bool correct = false;
		while (!correct)
		{
			scalar angle = rand() % 360;
			angle *= (2 * M_PI / 360);
			scalar distance = rand() % SPAWN_RADIUS;
			position.x = sin(angle) * distance;
			position.y = cos(angle) * distance;
		}*/
		break;
	}
	case InitialDistribution::SQUARE_UNIFORM:
	{
		break;
	}
	}

	return position;
}

PhysBody * Application::GetHeaviest()
{
	uint64 mass = 0;
	PhysBody* heaviest = nullptr;
	/*for (std::list<PhysBody*>::iterator it = rocks.begin(); it != rocks.end(); it++)
	{
		if ((*it)->mass > mass)
		{
			heaviest = *it;
			mass = (*it)->mass;
		}
	}*/
	for (int i = 0; i < MAX_BODIES; i++)
	{
		if (rocks[i].mass > mass)
		{
			heaviest = &rocks[i];
			mass = rocks[i].mass;
		}
	}
	return heaviest;
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
			case SDLK_KP_PLUS:
			{
				timescale *= (scalar)1.1;
				std::string output = "Time Scale: " + std::to_string(timescale) + "\n";
				OutputDebugString(output.c_str()); 
				break;
			}
			case SDLK_KP_MINUS:
			{
				timescale /= (scalar)1.1;
				std::string output = "Time Scale: " + std::to_string(timescale) + "\n";
				OutputDebugString(output.c_str());
				break;
			}
			case SDLK_g: G_FORCE = true; break;
			case SDLK_q: doStep = true; break;
			case SDLK_r: reset = true; break;
			case SDLK_e: timescale = 0.1; break;
			case SDLK_f: camera->SetScale(1.0); break;
			case SDLK_d: camera->Move({ 1000.0, 0.0 }); break;
			case SDLK_a: camera->Move({ -1000.0, 0.0 }); break;
			case SDLK_w: camera->Move({ 0.0, -1000.0 }); break;
			case SDLK_s: camera->Move({ 0.0, 1000.0 }); break;
			case SDLK_SPACE: paused = !paused; break;
			case SDLK_b: camera->SetTarget(GetHeaviest()); break;
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
			switch (event.button.button)
			{
			case 1:
				selectionRect.x = event.motion.x;
				selectionRect.y = event.motion.y;
				drawSelectionRect = true;
				break;
			case 2:
				movementDrag = true;
				break;
			case 3:
				/*for (std::list<PhysBody*>::iterator it = rocks.begin(); it != rocks.end(); it++)
				{
					if (!(*it)->active)
					{
						svec2 worldPos = { (scalar)event.motion.x , (scalar)event.motion.y };
						(*it)->pos = camera->ScreenToWorld(worldPos);
						(*it)->speed.x = 0;
						(*it)->speed.y = 0;
						(*it)->active = true;
						break;
					}
				}*/
				for (int i = 0; i < MAX_BODIES; i++)
				{
					if (!rocks[i].active)
					{
						svec2 worldPos = { (scalar)event.motion.x , (scalar)event.motion.y };
						rocks[i].pos = camera->ScreenToWorld(worldPos);
						rocks[i].speed.x = 0;
						rocks[i].speed.y = 0;
						rocks[i].active = true;
						break;
					}
				}
				break;
			}
		}
		else if (event.type == SDL_MOUSEBUTTONUP)
		{
			switch (event.button.button)
			{
			case 1:
			{
				iRect select = camera->ScreenToWorld(selectionRect.Normalised());

				if (select.w == 0) select.w++;
				if (select.h == 0) select.h++;
				camera->SetTarget(nullptr);

				/*for (std::list<PhysBody*>::iterator it = rocks.begin(); it != rocks.end(); it++)
				{
					if ((*it)->active)
					{
						SDL_Rect rock = {
							(int)((*it)->circle.x - (int)(*it)->circle.radius),
							(int)((*it)->circle.y - (int)(*it)->circle.radius),
							max((int)((*it)->circle.radius * 2), 1),
							max((int)((*it)->circle.radius * 2), 1)
						};
						SDL_Rect result;
						if (SDL_IntersectRect(&(SDL_Rect)select, &rock, &result) == SDL_TRUE)
						{
							camera->SetSpeed(0.0);
							camera->SetTarget(*it);
							std::string output = "Target position X:" + std::to_string((*it)->pos.x) + ", Y: " + std::to_string((*it)->pos.y) + "\n";
							OutputDebugString(output.c_str());
							break;
						}
					}
				}*/

				for (int i = 0; i < MAX_BODIES; i++)
				{
					if (rocks[i].active)
					{
						iRect rock = {
							(int)(rocks[i].circle.x - (int)rocks[i].circle.radius),
							(int)(rocks[i].circle.y - (int)rocks[i].circle.radius),
							max((int)(rocks[i].circle.radius * 2), 1),
							max((int)(rocks[i].circle.radius * 2), 1)
						};
						if (Utils::IntersectRect(select, rock))
						{
							camera->SetSpeed(0.0);
							camera->SetTarget(&rocks[i]);
							std::string output = "Target position X:" + std::to_string(rocks[i].pos.x) + ", Y: " + std::to_string(rocks[i].pos.y) + "\n";
							OutputDebugString(output.c_str());
							break;
						}
					}
				}

				drawSelectionRect = false;
				selectionRect = { -1,-1,0,0 };
				break;
			}
			case 2:
				movementDrag = false;
				break;
			}
		}
		else if (event.type == SDL_MOUSEMOTION)
		{
			if (drawSelectionRect)
			{
				selectionRect.w += event.motion.xrel;
				selectionRect.h += event.motion.yrel;
			}
			else if (movementDrag)
			{
				svec2 displacement = svec2(-event.motion.xrel, -event.motion.yrel);
				camera->SetPosition(camera->GetPosition() + displacement / camera->GetScale());
			}
		}
		else if (event.type == SDL_MOUSEWHEEL)
		{
			if (event.wheel.y > 0)
			{
				camera->SetScale(camera->GetScale() / (scalar)1.1);
			}
			else
			{
				camera->SetScale(camera->GetScale() * (scalar)1.1);
			}

			//std::string output = "Scale: " + std::to_string(camera->GetScale()) + "\n";
			//OutputDebugString(output.c_str());
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

	std::string output = "realDeltaTime: " + std::to_string(realDeltaTime) + "\n";
	OutputDebugString(output.c_str());

	//OutputDebugString("----------- Starting frame -----------\n");

	/*for (std::list<PhysBody*>::iterator it = trash.begin(); it != trash.end(); it++)
	{
		rocks.remove(*it);
		delete[] * it;
	}
	trash.clear();

	if (!paused || doStep)
	{
		for (std::list<PhysBody*>::iterator it = rocks.begin(); it != rocks.end(); it++)
		{
			if ((*it)->active)
			{
				(*it)->force = svec2(0.0, 0.0);
			}
		}
	}*/
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

	if (!paused || doStep)
	{
		doStep = false;

		unsigned int activeBodies = 0;
		scalar gForce = 0;
		svec2 distance;
		svec2 acceleration;
		PhysBody* target = camera->GetTarget();
		bool collision = false;
		int i, j;

		for (i = 0; i < MAX_BODIES; ++i)
		{
			if (rocks[i].active)
			{
				activeBodies++;
				for (j = i + 1; j < MAX_BODIES; ++j)
				{
					if (rocks[j].active)
					{
						//Removed sqrt() and instead squared the radii sum
						collision = (rocks[i].circle.x - rocks[j].circle.x) * (rocks[i].circle.x - rocks[j].circle.x)
							+ (rocks[i].circle.y - rocks[j].circle.y) * (rocks[i].circle.y - rocks[j].circle.y)
							< (rocks[i].circle.radius + rocks[j].circle.radius) * (rocks[i].circle.radius + rocks[j].circle.radius);

						if (collision)
						{
							if ((rocks[i].mass >= rocks[j].mass && &rocks[j] != target) || &rocks[i] == target)
							{
								rocks[j].active = false;
								long double total_mass = (long double)(rocks[i].mass + rocks[j].mass);
								rocks[i].density = (rocks[i].density * rocks[i].mass + rocks[j].density * rocks[j].mass) / total_mass;
								rocks[i].speed = ((rocks[i].speed * rocks[i].mass) + (rocks[j].speed * rocks[j].mass)) / (rocks[j].mass + rocks[i].mass);
								rocks[i].pos = ((rocks[i].pos * rocks[i].mass) + (rocks[j].pos * rocks[j].mass)) / (rocks[i].mass + rocks[j].mass);
								rocks[i].mass = (unsigned long long)total_mass;
								rocks[i].area = rocks[i].mass / rocks[i].density;
								rocks[i].circle.radius = sqrt(rocks[i].area / M_PI);
								rocks[i].diametre = 2 * rocks[i].circle.radius;
							}
							else
							{
								rocks[i].active = false;
								long double total_mass = (long double)(rocks[i].mass + rocks[j].mass);
								rocks[j].density = (rocks[j].density * rocks[j].mass + rocks[i].density * rocks[i].mass) / total_mass;
								rocks[j].speed = ((rocks[i].speed * rocks[i].mass) + (rocks[j].speed * rocks[j].mass)) / (rocks[j].mass + rocks[i].mass);
								rocks[j].pos = ((rocks[i].pos * rocks[i].mass) + (rocks[j].pos * rocks[j].mass)) / (rocks[i].mass + rocks[j].mass);
								rocks[j].mass = (unsigned long long)total_mass;
								rocks[j].area = rocks[j].mass / rocks[j].density;
								rocks[j].circle.radius = sqrt(rocks[j].area / M_PI);
								rocks[j].diametre = 2 * rocks[j].circle.radius;
							}
						}
						else
						{
							distance = rocks[j].pos - rocks[i].pos;

							gForce = ((G_CONSTANT * rocks[j].mass * rocks[i].mass) / max(distance.sqrLength(), rocks[i].circle.radius + rocks[j].circle.radius));

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

				acceleration = (rocks[i].force / rocks[i].mass);
				rocks[i].speed += acceleration * deltaTime;

				rocks[i].pos += (rocks[i].speed * deltaTime);
				rocks[i].UpdateTrail();

				//Friction (buggy)
				//rocks[i].speed = rocks[i].speed - (rocks[i].speed * (.00000000001 * timescale));

				rocks[i].circle.x = (int)rocks[i].pos.x;
				rocks[i].circle.y = (int)rocks[i].pos.y;

				rocks[i].force = { 0.0,0.0 };
			}
		}

		totalActiveBodies = activeBodies;
	}

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

	SDL_FPoint points[MAX_CIRCLE_POINTS];
	uint point_number = 0;
	scalar factor = 1.f;
	scalar cameraScale = camera->GetScale();
	svec2 cameraPos = camera->GetPosition();
	iRect rockRect, cameraRect;
	rockRect.w = 1;
	rockRect.h = 1;
	cameraRect = camera->GetRect();
	PhysBody* body = camera->GetTarget();
	int roundedRadius = 0;

	if (body != nullptr)
	{
		svec2 targetPosRelative = (body->pos - cameraPos) * cameraScale + svec2(HALF_SCREEN_WIDTH, HALF_SCREEN_HEIGHT);
		/*SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
		SDL_RenderDrawLineF(renderer, targetPosRelative.x, targetPosRelative.y, targetPosRelative.x + (body->force.x / body->mass) * cameraScale, targetPosRelative.y + (body->force.y / body->mass) * cameraScale);
		SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);*/
		SDL_RenderDrawLineF(renderer, targetPosRelative.x, targetPosRelative.y, targetPosRelative.x + body->speed.x * cameraScale, targetPosRelative.y + body->speed.y * cameraScale);
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

		SDL_FPoint* trailPoints = body->sortedTrail();
		for (int i = 0; i < TRAIL_LENGTH; i++)
		{
			trailPoints[i] = (SDL_FPoint)camera->WorldToScreen((svec2)trailPoints[i]);
		}
		SDL_RenderDrawLinesF(renderer, trailPoints, TRAIL_LENGTH);
	}

	body = null;

	for (int i = 0; i < MAX_BODIES; i++)
	{
		if (!rocks[i].active)
			continue;

		body = &rocks[i];

		roundedRadius = max((int)round(body->circle.radius), 1);
		rockRect.x = body->pos.x - roundedRadius;
		rockRect.y = body->pos.y - roundedRadius;
		rockRect.w = rockRect.h = 2 * roundedRadius;
		if (Utils::IntersectRect(cameraRect, rockRect))
		{
			//SDL_SetRenderDrawColor(renderer, rocks[i].color.r, rocks[i].color.g, rocks[i].color.b, 255);

			if (body->diametre * cameraScale > 1)
			{
				point_number = min((uint)(body->diametre * cameraScale) + 1, (uint)MAX_CIRCLE_POINTS);

				factor = (float)M_PI / (point_number / 2.f);

				for (unsigned int j = 0; j < point_number; ++j)
				{
					points[j].x = ((body->pos.x + (body->diametre / 2) * cos(j * factor)) - cameraPos.x) * cameraScale + HALF_SCREEN_WIDTH;
					points[j].y = ((body->pos.y + (body->diametre / 2) * sin(j * factor)) - cameraPos.y) * cameraScale + HALF_SCREEN_HEIGHT;
				}
				SDL_RenderDrawPointsF(renderer, points, point_number);
			}
			else
			{
				SDL_RenderDrawPoint(renderer, (body->pos.x - cameraPos.x) * cameraScale + HALF_SCREEN_WIDTH, (body->pos.y - cameraPos.y) * cameraScale + HALF_SCREEN_HEIGHT);
			}
		}
	}

	if (drawSelectionRect)
	{
		SDL_RenderDrawRect(renderer, &(SDL_Rect)selectionRect);
	}

	// Finally swap buffers
	SDL_RenderPresent(renderer);

	/*output = "Time at Draw() end: " + std::to_string(frameTimeTimer.Readms()) + "\n";
	OutputDebugString(output.c_str());*/
}
