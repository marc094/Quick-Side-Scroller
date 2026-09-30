#include "Application.h"
#include <time.h>
#include <algorithm>
#include <cmath>
#include <string>
#include "PhysBody.h"
#include "Camera.h"
#ifdef _WIN32
#include <windows.h>
#endif
#include "Platform.h"

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
	std::uniform_int_distribution<std::mt19937_64::result_type> distributionMass(MIN_MASS, MAX_MASS);
	std::uniform_int_distribution<std::mt19937_64::result_type> distributionDensity(MIN_DENSITY, MAX_DENSITY);
	std::uniform_real_distribution<> distributionMomentum(MIN_INITIAL_MOMENTUM, MAX_INITIAL_MOMENTUM);
	std::uniform_real_distribution<> distributionMomentumDirection(0, 2*M_PI);

	body->mass = distributionMass(rng);
	//cel_body->diametre = rand() % (MAX_DIAMETRE - MIN_DIAMETRE + 1) + MIN_DIAMETRE;

	body->density = distributionDensity(rng);
	body->diametre = 2 * sqrt(body->mass / (M_PI*body->density));

	body->area = (body->diametre * 0.5) * (body->diametre * 0.5) * M_PI;
	//cel_body->density = cel_body->mass / cel_body->area;

	body->pos = GenerateInitialPosition();
	scalar momentum = distributionMomentum(rng);
	scalar direction = distributionMomentumDirection(rng);
	body->speed.x = (momentum / body->mass) * sin(direction);
	body->speed.y = (momentum / body->mass) * cos(direction);
	/*scalar speed = (rand() % (int)(STARTING_SPEED_MAGNITUDE * 0.1) + STARTING_SPEED_MAGNITUDE) * sqrt(distance) / body->mass;
	body->speed.x = speed * sin(angle + M_PI / 2);
	body->speed.y = speed * cos(angle + M_PI / 2);*/
	/*float factor = (float)M_PI / (MAX_BODIES * 0.5f);
	cel_body->pos.x = (double)((SCREEN_WIDTH / 2) + (400) * cos(index * factor));
	cel_body->pos.y = (double)((SCREEN_HEIGHT / 2) + (400) * sin(index * factor));*/
	
	//body->speed = sZero;
	body->force = sZero;

	body->color = { 255, 255, 255 };

	body->circle.x = (int)body->pos.x;
	body->circle.y = (int)body->pos.y;
	body->circle.radius = body->diametre / 2;
	body->active = true;
}

void Application::Start()
{
	SDL_Init(SDL_INIT_EVERYTHING);

	// Create window & renderer
	window = SDL_CreateWindow("Stellar", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_BORDERLESS);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == nullptr)
		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);

	camera = new Camera();

	// Load image lib --
	//IMG_Init(IMG_INIT_PNG);

	// Create mixer --
	/*Mix_Init(MIX_INIT_OGG);
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);*/

	rocks = ARR_DECL(PhysBody, MAX_BODIES);
	trail.resize(TRAIL_LENGTH);
	trailScreen.resize(TRAIL_LENGTH);
	frameTimeTimer.Start();
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
	forcesValid = false;
	trailOwner = nullptr;

	reset = false;
}

// Prints values that should be conserved by the simulation (total momentum, total mass)
void Application::PrintStats() const
{
	svec2 momentum = sZero;
	scalar totalMass = 0;
	uint active = 0;

	for (int i = 0; i < MAX_BODIES; ++i)
	{
		if (!rocks[i].active)
			continue;

		momentum += rocks[i].speed * rocks[i].mass;
		totalMass += (scalar)rocks[i].mass;
		active++;
	}

	char output[256];
	snprintf(output, sizeof(output), "active=%u mass=%.6e momentum=(%.6e, %.6e)\n", active, totalMass, momentum.x, momentum.y);
	OutputDebugString(output);
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
	std::uniform_real_distribution<scalar> unit(0.0, 1.0);
	std::uniform_real_distribution<scalar> signedUnit(-1.0, 1.0);
	svec2 position = sZero;

	switch (INITIAL_DISTRIBUTION)
	{
	case InitialDistribution::RADIAL_CENTRE_DENSER:
	{
		scalar angle = unit(rng) * 2 * M_PI;
		scalar distance = unit(rng) * SPAWN_RADIUS;
		position.x = sin(angle) * distance;
		position.y = cos(angle) * distance;
		break;
	}
	case InitialDistribution::RADIAL_UNIFORM:
	{
		scalar angle = unit(rng) * 2 * M_PI;
		scalar distance = sqrt(unit(rng)) * SPAWN_RADIUS;
		position.x = sin(angle) * distance;
		position.y = cos(angle) * distance;
		break;
	}
	case InitialDistribution::SQUARE_CENTRE_DENSER:
	{
		// Squaring biases each coordinate towards 0
		scalar x = signedUnit(rng), y = signedUnit(rng);
		position.x = x * fabs(x) * SPAWN_RADIUS;
		position.y = y * fabs(y) * SPAWN_RADIUS;
		break;
	}
	case InitialDistribution::SQUARE_UNIFORM:
	{
		position.x = signedUnit(rng) * SPAWN_RADIUS;
		position.y = signedUnit(rng) * SPAWN_RADIUS;
		break;
	}
	}

	return position;
}

PhysBody * Application::GetHeaviest()
{
	uint64 mass = 0;
	PhysBody* heaviest = nullptr;
	for (int i = 0; i < MAX_BODIES; i++)
	{
		if (rocks[i].active && rocks[i].mass > mass)
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
						InitBody(&rocks[i]);
						rocks[i].pos = camera->ScreenToWorld(svec2((scalar)event.motion.x, (scalar)event.motion.y));
						rocks[i].speed = sZero;
						rocks[i].circle.x = (sint64)rocks[i].pos.x;
						rocks[i].circle.y = (sint64)rocks[i].pos.y;
						forcesValid = false;
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
	// Clamp so a stall (window drag, breakpoint) doesn't produce a giant simulation step
	realDeltaTime = frameTimeTimer.ReadSec();
	if (realDeltaTime > 0.1f)
		realDeltaTime = 0.1f;
	deltaTime = realDeltaTime * timescale;

	frameTimeTimer.Start();


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
void Application::MergeBodies(PhysBody& survivor, PhysBody& absorbed)
{
	scalar totalMass = (scalar)survivor.mass + (scalar)absorbed.mass;

	survivor.density = (survivor.density * survivor.mass + absorbed.density * absorbed.mass) / totalMass;
	survivor.speed = ((survivor.speed * survivor.mass) + (absorbed.speed * absorbed.mass)) / totalMass;
	survivor.pos = ((survivor.pos * survivor.mass) + (absorbed.pos * absorbed.mass)) / totalMass;
	survivor.force += absorbed.force;
	survivor.mass += absorbed.mass;
	survivor.area = survivor.mass / survivor.density;
	survivor.circle.radius = sqrt(survivor.area / M_PI);
	survivor.diametre = 2 * survivor.circle.radius;
	survivor.circle.x = (sint64)survivor.pos.x;
	survivor.circle.y = (sint64)survivor.pos.y;

	absorbed.active = false;
	absorbed.force = sZero;
}

// Resolves collisions and accumulates the gravitational force on every body.
// Positions are not modified here (other than by merges), so every pair sees a
// consistent snapshot of the system.
void Application::ComputeForces()
{
	PhysBody* target = camera->GetTarget();
	uint activeBodies = 0;

	for (int i = 0; i < MAX_BODIES; ++i)
		rocks[i].force = sZero;

	for (int i = 0; i < MAX_BODIES; ++i)
	{
		if (!rocks[i].active)
			continue;

		// rocks[i] can be absorbed mid-loop, in which case it has nothing left to do
		for (int j = i + 1; j < MAX_BODIES && rocks[i].active; ++j)
		{
			if (!rocks[j].active)
				continue;

			PhysBody& a = rocks[i];
			PhysBody& b = rocks[j];

			svec2 distance = b.pos - a.pos;
			scalar sqrDistance = distance.sqrLength();
			scalar radiusSum = a.circle.radius + b.circle.radius;

			// Compared squared to avoid the sqrt()
			if (sqrDistance < radiusSum * radiusSum)
			{
				if ((a.mass >= b.mass && &b != target) || &a == target)
					MergeBodies(a, b);
				else
					MergeBodies(b, a);
			}
			else
			{
				scalar gForce = (G_CONSTANT * b.mass * a.mass) / sqrDistance;

				distance.normalise();

				a.force += distance * gForce;
				b.force -= distance * gForce;
			}
		}

		if (!rocks[i].active)
			continue;

		activeBodies++;

		if (G_FORCE)
		{
			svec2 delta = rocks[i].pos - svec2(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
			delta.normalise();
			rocks[i].force -= delta * G_FORCE_STRENGTH;
		}
	}

	totalActiveBodies = activeBodies;
}

// Kick-drift-kick leapfrog. Relies on `force` holding the forces from the end of the previous step.
void Application::Step(scalar dt)
{
	const scalar halfDt = dt * 0.5;

	for (int i = 0; i < MAX_BODIES; ++i)
	{
		if (!rocks[i].active)
			continue;

		rocks[i].speed += (rocks[i].force / rocks[i].mass) * halfDt;
		rocks[i].pos += rocks[i].speed * dt;
	}

	ComputeForces();

	for (int i = 0; i < MAX_BODIES; ++i)
	{
		if (!rocks[i].active)
			continue;

		rocks[i].speed += (rocks[i].force / rocks[i].mass) * halfDt;
		rocks[i].circle.x = (sint64)rocks[i].pos.x;
		rocks[i].circle.y = (sint64)rocks[i].pos.y;
	}
}

// The trail only exists for the camera target
void Application::UpdateTrail(bool advance)
{
	PhysBody* target = camera->GetTarget();

	if (target == nullptr || !target->active)
	{
		trailOwner = nullptr;
		return;
	}

	if (target != trailOwner)
	{
		trailOwner = target;
		trailIndex = 0;
		trailTimer = 0;
		std::fill(trail.begin(), trail.end(), target->pos);
	}

	if (!advance)
		return;

	trailTimer += deltaTime;
	if (trailTimer > TRAIL_UPDATE_FREQUENCY)
	{
		trailTimer = 0;
		trailIndex = (trailIndex + TRAIL_LENGTH - 1) % TRAIL_LENGTH;
		trail[trailIndex] = target->pos;
	}
}

// -----------------------------------------------------------------
void Application::Update()
{
	if (reset)
		Reset();

	bool advance = !paused || doStep;
	if (advance)
	{
		doStep = false;

		if (!forcesValid)
		{
			ComputeForces();
			forcesValid = true;
		}

		// Subdivide big frames so fast timescales don't degrade the integration
		scalar dt = deltaTime;
		int steps = (int)Utils::Clamp(ceil(dt / MAX_PHYSICS_STEP), 1, MAX_SUBSTEPS);
		for (int s = 0; s < steps; ++s)
			Step(dt / steps);
	}

	camera->Update();
	UpdateTrail(advance);
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
		SDL_RenderDrawLineF(renderer, targetPosRelative.x, targetPosRelative.y, targetPosRelative.x + body->speed.x * cameraScale * VELOCITY_VECTOR_SCALE, targetPosRelative.y + body->speed.y * cameraScale * VELOCITY_VECTOR_SCALE);
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

		if (trailOwner == body)
		{
			for (int i = 0; i < TRAIL_LENGTH; i++)
			{
				trailScreen[i] = (SDL_FPoint)camera->WorldToScreen(trail[(trailIndex + i) % TRAIL_LENGTH]);
			}
			SDL_RenderDrawLinesF(renderer, trailScreen.data(), TRAIL_LENGTH);
		}
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
		SDL_Rect selection = (SDL_Rect)selectionRect;
		SDL_RenderDrawRect(renderer, &selection);
	}

	// Finally swap buffers
	SDL_RenderPresent(renderer);

	/*output = "Time at Draw() end: " + std::to_string(frameTimeTimer.Readms()) + "\n";
	OutputDebugString(output.c_str());*/
}
