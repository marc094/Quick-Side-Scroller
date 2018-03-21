#include "Defs.h"
#include <time.h>
#include <random>

struct globals
{
	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;
	CelestialBody rocks[MAX_BODIES];
	bool G_FORCE = false;
	double timescale = 1;
	bool reset = false;
	dRect camera = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
	double cameraSpeed = 0;
	CelestialBody* trackingBody = nullptr;
	iRect selectionRect = { 0,0,0,0 };
	bool drawSelectionRect = false;
	unsigned int totalActiveBodies = 0;
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
	dvec2 displacement = totalDisplacement.normalise() * step;
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
	cel_body->diametre = rand() % (MAX_DIAMETRE - MIN_DIAMETRE + 1) + MIN_DIAMETRE;
	double pi = M_PI;

	cel_body->volume = pow(cel_body->diametre * 0.5, 2) * pi;
	cel_body->density = cel_body->mass / cel_body->volume;

	//double r = rand() % SCREEN_WIDTH;
	cel_body->pos.x = rand() % (3 * SCREEN_WIDTH) - SCREEN_WIDTH;
	cel_body->pos.y = rand() % (3 * SCREEN_HEIGHT) - SCREEN_HEIGHT;
	cel_body->speed.x = 0;
	cel_body->speed.y = 0;
	/*float factor = (float)M_PI / (MAX_BODIES * 0.5f);
	cel_body->pos.x = (double)((SCREEN_WIDTH / 2) + (400) * cos(index * factor));
	cel_body->pos.y = (double)((SCREEN_HEIGHT / 2) + (400) * sin(index * factor));*/
	cel_body->color = { 255, 255, 255 };

	cel_body->circle.x = cel_body->pos.x;
	cel_body->circle.y = cel_body->pos.y;
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
	Start();
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
			case SDLK_g: g.G_FORCE = !g.G_FORCE; break;
			case SDLK_r: g.reset = true;
			case SDLK_RIGHT: g.camera.x+=10; break;
			case SDLK_LEFT: g.camera.x-=10; break;
			case SDLK_UP: g.camera.y-=10; break;
			case SDLK_DOWN: g.camera.y+=10; break;
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
			select.x += g.camera.x;
			select.y += g.camera.y;
			if (select.w == 0) select.w++;
			if (select.h == 0) select.h++;
			g.trackingBody = nullptr;

			for (int i = 0; i < MAX_BODIES; ++i)
			{
				if (g.rocks[i].active)
				{
					SDL_Rect rock = { g.rocks[i].circle.x - g.rocks[i].circle.radius, g.rocks[i].circle.y - g.rocks[i].circle.radius , g.rocks[i].circle.radius * 2 , g.rocks[i].circle.radius * 2};
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
void Update()
{
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
					bool col = sqrt(pow(g.rocks[i].circle.x - g.rocks[j].circle.x, 2) + pow(g.rocks[i].circle.y - g.rocks[j].circle.y, 2)) < (g.rocks[i].circle.radius + g.rocks[j].circle.radius)/* * 0.8*/;
					if (col) {
						if (g.rocks[i].mass >= g.rocks[j].mass)
						{
							g.rocks[j].active = false;
							long double total_mass = (long double)(g.rocks[i].mass + g.rocks[j].mass);
							g.rocks[i].density = (g.rocks[i].density * (g.rocks[i].mass / total_mass)) + (g.rocks[j].density * (g.rocks[j].mass / total_mass));
							g.rocks[i].mass = total_mass;
							g.rocks[i].diametre = 2 * sqrt((g.rocks[i].mass) / (M_PI * g.rocks[i].density));
							g.rocks[i].circle.radius = g.rocks[i].diametre / 2;
							g.rocks[i].volume = M_PI * pow(g.rocks[i].circle.radius, 2);
							g.rocks[i].speed.x = (((g.rocks[i].speed.x * g.rocks[i].mass) + (g.rocks[j].speed.x * g.rocks[j].mass)) / (g.rocks[j].mass + g.rocks[i].mass));
							g.rocks[i].speed.y = (((g.rocks[i].speed.y * g.rocks[i].mass) + (g.rocks[j].speed.y * g.rocks[j].mass)) / (g.rocks[j].mass + g.rocks[i].mass));
						}
						else
						{
							g.rocks[i].active = false;
							long double total_mass = (long double)(g.rocks[i].mass + g.rocks[j].mass);
							g.rocks[j].density = (g.rocks[i].density * (g.rocks[i].mass / total_mass)) + (g.rocks[j].density * (g.rocks[j].mass / total_mass));
							g.rocks[j].mass = total_mass;
							g.rocks[j].diametre = 2 * sqrt((g.rocks[j].mass) / (M_PI * g.rocks[j].density));
							g.rocks[j].circle.radius = g.rocks[j].diametre / 2;
							g.rocks[j].volume = M_PI * pow(g.rocks[j].circle.radius, 2);
							g.rocks[j].speed.x = (((g.rocks[i].speed.x * g.rocks[i].mass) + (g.rocks[j].speed.x * g.rocks[j].mass)) / (g.rocks[j].mass + g.rocks[i].mass));
							g.rocks[j].speed.y = (((g.rocks[i].speed.y * g.rocks[i].mass) + (g.rocks[j].speed.y * g.rocks[j].mass)) / (g.rocks[j].mass + g.rocks[i].mass));
						}
					}
					double G = 6.673 * pow(10, -11);
					double gForce = ((G * g.rocks[j].mass * g.rocks[i].mass) / pow(g.rocks[i].pos.distance(g.rocks[j].pos), 2));
					double angle = g.rocks[i].pos.angle(g.rocks[j].pos);
					g.rocks[i].speed.x -= ((gForce / g.rocks[i].mass) * cos(angle)) * g.timescale;
					g.rocks[i].speed.y -= ((gForce / g.rocks[i].mass) * sin(angle)) * g.timescale;
					g.rocks[j].speed.x += ((gForce / g.rocks[j].mass) * cos(angle)) * g.timescale;
					g.rocks[j].speed.y += ((gForce / g.rocks[j].mass) * sin(angle)) * g.timescale;
				}
			}

			if (g.G_FORCE)
			{
				double angle = g.rocks[i].pos.angle({SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2});
				g.rocks[i].speed.x -= (1000000 / g.rocks[i].mass) * cos(angle);// *g.timescale;
				g.rocks[i].speed.y -= (1000000 / g.rocks[i].mass) * sin(angle);// *g.timescale;
			}

			g.rocks[i].pos.x += (g.rocks[i].speed.x * g.timescale);
			g.rocks[i].pos.y += (g.rocks[i].speed.y * g.timescale);

			//Friction (buggy)
			//g.rocks[i].speed = g.rocks[i].speed - (g.rocks[i].speed * (.00000000001 * g.timescale));

			g.rocks[i].circle.x = g.rocks[i].pos.x;
			g.rocks[i].circle.y = g.rocks[i].pos.y;
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
	}
	else g.cameraSpeed = 0;
}

// ----------------------------------------------------------------
void Draw()
{
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
			SDL_Point points[CIRCLE_POINTS];

			float factor = (float)M_PI / (CIRCLE_POINTS / 2.f);

			for (unsigned int j = 0; j < CIRCLE_POINTS; ++j)
			{
				points[j].x = (int)(g.rocks[i].pos.x + (g.rocks[i].diametre / 2) * cos(j * factor)) - g.camera.x;
				points[j].y = (int)(g.rocks[i].pos.y + (g.rocks[i].diametre / 2) * sin(j * factor)) - g.camera.y;
			}

			result = SDL_RenderDrawPoints(g.renderer, points, CIRCLE_POINTS);
		}
		else {
			SDL_Point p = { g.rocks[i].pos.x - g.camera.x, g.rocks[i].pos.y - g.camera.y };
			SDL_RenderDrawPoints(g.renderer, &p, 1);
		}
	}

	if (g.drawSelectionRect)
	{
		SDL_RenderDrawRect(g.renderer, &g.selectionRect.toSDL());
	}

	// Finally swap buffers
	SDL_RenderPresent(g.renderer);
}