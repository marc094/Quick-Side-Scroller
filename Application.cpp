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

	body->density = distributionDensity(rng);
	body->diametre = 2 * sqrt(body->mass / (M_PI*body->density));

	body->area = (body->diametre * 0.5) * (body->diametre * 0.5) * M_PI;

	body->pos = GenerateInitialPosition();
	scalar momentum = distributionMomentum(rng);
	scalar direction = distributionMomentumDirection(rng);
	body->speed.x = (momentum / body->mass) * sin(direction);
	body->speed.y = (momentum / body->mass) * cos(direction);
	
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

	rocks = ARR_DECL(PhysBody, MAX_BODIES);
	trail.resize(TRAIL_LENGTH);
	trailScreen.resize(TRAIL_LENGTH);
	frameTimeTimer.Start();

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

void Application::SetZoom(scalar scale)
{
	camera->SetScale(scale);
}

// Saves what the renderer last drew as a BMP
bool Application::SaveScreenshot(const char* path)
{
	SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, SDL_PIXELFORMAT_ARGB8888);
	if (surface == nullptr)
		return false;

	bool ok = SDL_RenderReadPixels(renderer, nullptr, surface->format->format, surface->pixels, surface->pitch) == 0
		&& SDL_SaveBMP(surface, path) == 0;
	SDL_FreeSurface(surface);
	return ok;
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
	ARR_FREE(rocks);

	delete camera;
	camera = nullptr;

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

// Merges every pair of overlapping bodies. Candidates come from the quadtree; the actual
// overlap is re-checked against live values because merging changes position and radius.
void Application::ResolveCollisions()
{
	PhysBody* target = camera->GetTarget();
	bool merged = false;

	tree.Build(rocks, MAX_BODIES);

	for (int i = 0; i < MAX_BODIES; ++i)
	{
		if (!rocks[i].active)
			continue;

		candidates.clear();
		tree.QueryOverlaps(rocks, i, candidates);

		// rocks[i] can be absorbed mid-loop, in which case it has nothing left to do
		for (size_t c = 0; c < candidates.size() && rocks[i].active; ++c)
		{
			PhysBody& a = rocks[i];
			PhysBody& b = rocks[candidates[c]];
			if (!b.active)
				continue;

			scalar radiusSum = a.circle.radius + b.circle.radius;

			// Compared squared to avoid the sqrt()
			if (a.pos.sqrDistance(b.pos) < radiusSum * radiusSum)
			{
				if ((a.mass >= b.mass && &b != target) || &a == target)
					MergeBodies(a, b);
				else
					MergeBodies(b, a);
				merged = true;
			}
		}
	}

	// Merging moved and removed bodies, so the tree no longer matches
	treeValid = !merged;
}

// Resolves collisions and accumulates the gravitational force on every body.
// Positions are not modified here (other than by merges), so every body sees a
// consistent snapshot of the system.
void Application::ComputeForces()
{
	ResolveCollisions();

	if (exactForces)
	{
		AccumulateForcesDirect();
	}
	else
	{
		if (!treeValid)
			tree.Build(rocks, MAX_BODIES);

		#pragma omp parallel for schedule(dynamic, 64)
		for (int i = 0; i < MAX_BODIES; ++i)
		{
			rocks[i].force = rocks[i].active ? tree.Force(rocks, i, BH_THETA) : sZero;
		}
	}

	uint activeBodies = 0;
	for (int i = 0; i < MAX_BODIES; ++i)
	{
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

// Exact O(n^2) forces, used as the reference for the tree approximation
void Application::AccumulateForcesDirect()
{
	for (int i = 0; i < MAX_BODIES; ++i)
		rocks[i].force = sZero;

	for (int i = 0; i < MAX_BODIES; ++i)
	{
		if (!rocks[i].active)
			continue;

		for (int j = i + 1; j < MAX_BODIES; ++j)
		{
			if (!rocks[j].active)
				continue;

			PhysBody& a = rocks[i];
			PhysBody& b = rocks[j];

			svec2 distance = b.pos - a.pos;
			scalar sqrDistance = distance.sqrLength();
			if (sqrDistance <= 0)
				continue;

			// Same softening as the tree for overlapping bodies
			scalar radiusSum = a.circle.radius + b.circle.radius;
			scalar softened = std::max(sqrDistance, radiusSum * radiusSum);
			svec2 force = distance * ((G_CONSTANT * a.mass * b.mass) / (softened * sqrt(sqrDistance)));

			a.force += force;
			b.force -= force;
		}
	}
}

// Compares the tree forces against the exact ones on the current state
void Application::PrintForceError()
{
	ResolveCollisions();
	tree.Build(rocks, MAX_BODIES);
	treeValid = true;

	std::vector<svec2> approximate(MAX_BODIES);
	for (int i = 0; i < MAX_BODIES; ++i)
		approximate[i] = rocks[i].active ? tree.Force(rocks, i, BH_THETA) : sZero;

	AccumulateForcesDirect();

	scalar sum = 0, worst = 0;
	uint count = 0;
	for (int i = 0; i < MAX_BODIES; ++i)
	{
		if (!rocks[i].active)
			continue;

		scalar length = rocks[i].force.length();
		if (length <= 0)
			continue;

		scalar error = (approximate[i] - rocks[i].force).length() / length;
		sum += error;
		worst = std::max(worst, error);
		count++;
	}

	char output[256];
	snprintf(output, sizeof(output), "theta=%.2f force error: mean=%.4f%% max=%.4f%% (%u bodies)\n", (double)BH_THETA, 100 * sum / std::max(count, 1u), 100 * worst, count);
	OutputDebugString(output);
	forcesValid = false;
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
void Application::FlushPoints()
{
	if (!drawPoints.empty())
		SDL_RenderDrawPointsF(renderer, drawPoints.data(), (int)drawPoints.size());
	drawPoints.clear();
}

void Application::Draw()
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	const scalar cameraScale = camera->GetScale();
	const svec2 cameraPos = camera->GetPosition();
	PhysBody* target = camera->GetTarget();

	if (target != nullptr)
	{
		svec2 targetPosRelative = (target->pos - cameraPos) * cameraScale + svec2(HALF_SCREEN_WIDTH, HALF_SCREEN_HEIGHT);
		SDL_RenderDrawLineF(renderer, targetPosRelative.x, targetPosRelative.y, targetPosRelative.x + target->speed.x * cameraScale * VELOCITY_VECTOR_SCALE, targetPosRelative.y + target->speed.y * cameraScale * VELOCITY_VECTOR_SCALE);

		if (trailOwner == target)
		{
			for (int i = 0; i < TRAIL_LENGTH; i++)
			{
				trailScreen[i] = (SDL_FPoint)camera->WorldToScreen(trail[(trailIndex + i) % TRAIL_LENGTH]);
			}
			SDL_RenderDrawLinesF(renderer, trailScreen.data(), TRAIL_LENGTH);
		}
	}

	// All bodies go through one point buffer so SDL gets a few big calls instead of one per body
	drawPoints.clear();

	for (int i = 0; i < MAX_BODIES; i++)
	{
		const PhysBody& body = rocks[i];
		if (!body.active)
			continue;

		// Culled in screen space, before doing any per-body work
		const scalar screenX = (body.pos.x - cameraPos.x) * cameraScale + HALF_SCREEN_WIDTH;
		const scalar screenY = (body.pos.y - cameraPos.y) * cameraScale + HALF_SCREEN_HEIGHT;
		const scalar screenDiametre = body.diametre * cameraScale;
		const scalar margin = screenDiametre * 0.5 + 1;
		if (screenX < -margin || screenX > SCREEN_WIDTH + margin || screenY < -margin || screenY > SCREEN_HEIGHT + margin)
			continue;

		if (screenDiametre > 1)
		{
			// Circle outline, one vertex per screen pixel of diametre. Vertices are stepped by
			// rotating a vector instead of calling sin/cos for each one.
			const uint pointCount = min((uint)screenDiametre + 1, (uint)MAX_CIRCLE_POINTS);
			const scalar step = 2 * M_PI / pointCount;
			const scalar stepCos = cos(step), stepSin = sin(step);
			const scalar radius = screenDiametre * 0.5;
			scalar x = radius, y = 0;

			for (uint j = 0; j < pointCount; ++j)
			{
				drawPoints.push_back({ (float)(screenX + x), (float)(screenY + y) });
				const scalar rotatedX = x * stepCos - y * stepSin;
				y = x * stepSin + y * stepCos;
				x = rotatedX;
			}
		}
		else
		{
			drawPoints.push_back({ (float)screenX, (float)screenY });
		}

		if (drawPoints.size() >= DRAW_BATCH_SIZE)
			FlushPoints();
	}

	FlushPoints();

	if (drawSelectionRect)
	{
		SDL_Rect selection = (SDL_Rect)selectionRect;
		SDL_RenderDrawRect(renderer, &selection);
	}

	// Finally swap buffers
	SDL_RenderPresent(renderer);
}
