#include "Defs.h"
#include <time.h>
#include <random>

struct globals
{
	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;
	CelestialBody rocks[MAX_BODIES];
} g; // automatically create an insteance called "g"

bool CircleInCircle(Circle c1, Circle c2) {
	if (sqrt(pow(c1.x - c2.x, 2) + pow(c1.y - c2.y, 2)) < c1.radius + c2.radius)
		return true;
	return false;
}

bool CheckCollision(CelestialBody b1, CelestialBody b2)
{
	if (CircleInCircle(b1.circle, b2.circle))
		return true;
	return false;
}

void InitBody(int index) {
	CelestialBody* cel_body = &g.rocks[index];

	cel_body->density = rand() % (MAX_MASS - MIN_MASS) + MIN_MASS;
	cel_body->diametre = rand() % (MAX_DIAMETRE - MIN_DIAMETRE) + MIN_DIAMETRE;
	double pi = M_PI;
	cel_body->mass = cel_body->diametre * 0.5 * pi * pi * cel_body->density;
	//double r = rand() % SCREEN_WIDTH;
	cel_body->pos.x = rand() % SCREEN_WIDTH;
	cel_body->pos.y = rand () % SCREEN_HEIGHT;
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
	g.window = SDL_CreateWindow("QSS - Quick Side Scroller - 0.5", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_BORDERLESS);
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
			}
		}
		else if (event.type == SDL_QUIT)
			ret = false;
	}

	return ret;
}

// ----------------------------------------------------------------
void Update()
{
	for (int i = 0; i < MAX_BODIES; ++i)
	{
		if (g.rocks[i].active)
		{
			for (int j = i + 1; j < MAX_BODIES; ++j)
			{
				if (g.rocks[j].active)
				{
					bool col = sqrt(pow(g.rocks[i].circle.x - g.rocks[j].circle.x, 2) + pow(g.rocks[i].circle.y - g.rocks[j].circle.y, 2)) < g.rocks[i].circle.radius + g.rocks[j].circle.radius;
					if (col) {
						if (g.rocks[i].mass >= g.rocks[j].mass)
						{
							g.rocks[j].active = false;
							g.rocks[i].mass += g.rocks[j].mass;
							g.rocks[i].diametre = (2 * g.rocks[i].mass) / (M_PI * M_PI * g.rocks[i].density);
							g.rocks[i].circle.radius = g.rocks[i].diametre / 2;
							g.rocks[i].speed += g.rocks[j].speed * (g.rocks[j].mass / g.rocks[i].mass);
						}
						else
						{
							g.rocks[i].active = false;
							g.rocks[j].mass += g.rocks[i].mass;
							g.rocks[j].diametre = (2 * g.rocks[j].mass) / (M_PI * M_PI * g.rocks[j].density);
							g.rocks[j].circle.radius = g.rocks[j].diametre / 2;
							g.rocks[j].speed += g.rocks[i].speed * (g.rocks[i].mass / g.rocks[j].mass);
						}
					}
					double G = 6.673 * pow(10, -11);
					double gForce = (G * g.rocks[j].mass) / g.rocks[i].pos.distance(g.rocks[j].pos);
					double angle = g.rocks[i].pos.angle(g.rocks[j].pos);
					g.rocks[i].speed.x += gForce * cos(angle);
					g.rocks[i].speed.y += gForce * sin(angle);
					g.rocks[j].speed.x += -gForce * cos(angle);
					g.rocks[j].speed.y += -gForce * sin(angle);
				}
			}
			g.rocks[i].pos += g.rocks[i].speed;

			g.rocks[i].circle.x = g.rocks[i].pos.x;
			g.rocks[i].circle.y = g.rocks[i].pos.y;
		}
	}
}

// ----------------------------------------------------------------
void Draw()
{
	SDL_SetRenderDrawColor(g.renderer, 0, 0, 0, 255);
	SDL_RenderClear(g.renderer);

	SDL_SetRenderDrawColor(g.renderer, 255, 255, 255, 255);

	for (int i = 0; i < MAX_BODIES; ++i)
	{
		if (!g.rocks[i].active)
			continue;

		SDL_SetRenderDrawBlendMode(g.renderer, SDL_BLENDMODE_BLEND);
		//SDL_SetRenderDrawColor(g.renderer, g.rocks[i].color.r, g.rocks[i].color.g, g.rocks[i].color.b, 255);

		if (g.rocks[i].diametre > 1) {
			int result = -1;
			SDL_Point points[360];

			float factor = (float)M_PI / 180.0f;

			for (unsigned int j = 0; j < 360; ++j)
			{
				points[j].x = (int)(g.rocks[i].pos.x + (g.rocks[i].diametre / 2) * cos(j * factor));
				points[j].y = (int)(g.rocks[i].pos.y + (g.rocks[i].diametre / 2) * sin(j * factor));
			}

			result = SDL_RenderDrawPoints(g.renderer, points, 360);
		}
		else {
			SDL_Point p = { g.rocks[i].pos.x, g.rocks[i].pos.y };
			SDL_RenderDrawPoints(g.renderer, &p, 360);
		}
	}

	// Finally swap buffers
	SDL_RenderPresent(g.renderer);
}

const vec2& vec2::operator +(const vec2& other) {
	return { x + other.x, y + other.y };
}

const vec2& vec2::operator -(const vec2& other) {
	return { x - other.x, y - other.y };
}

const vec2& vec2::operator +=(const vec2& other) {
	x += other.x;
	y += other.y;
	return(*this);
}

const vec2& vec2::operator *(double scalar) {
	return { x * scalar, y * scalar };
}

float vec2::distance(const vec2& other) {
	return sqrt((other.x - x) * (other.x - x) + (other.y - y) * (other.y - y));
}

float vec2::angle(const vec2& other) {
	return acos((x * other.x + y * other.y) / (sqrt(x * x + y * y) * sqrt(other.x * other.x + other.y * other.y)));
}