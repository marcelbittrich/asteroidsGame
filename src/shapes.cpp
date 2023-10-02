#include "shapes.hpp"

//mid point circle algorithm based on https://en.wikipedia.org/w/index.php?title=Midpoint_circle_algorithm&oldid=889172082#C_example
void drawCircle(SDL_Renderer* renderer, int x0, int y0, int radius)
{
	int x = radius - 1;
	int y = 0;
	int dx = 1;
	int dy = 1;
	int err = dx - (radius << 1);

	while (x >= y)
	{
		SDL_RenderDrawPoint(renderer, x0 + x, y0 + y);
		SDL_RenderDrawPoint(renderer, x0 + y, y0 + x);
		SDL_RenderDrawPoint(renderer, x0 - y, y0 + x);
		SDL_RenderDrawPoint(renderer, x0 - x, y0 + y);
		SDL_RenderDrawPoint(renderer, x0 - x, y0 - y);
		SDL_RenderDrawPoint(renderer, x0 - y, y0 - x);
		SDL_RenderDrawPoint(renderer, x0 + y, y0 - x);
		SDL_RenderDrawPoint(renderer, x0 + x, y0 - y);

		if (err <= 0)
		{
			y++;
			err += dy;
			dy += 2;
		}

		if (err > 0)
		{
			x--;
			dx += 2;
			err += dx - (radius << 1);
		}
	}
}

void drawTriangle(SDL_Renderer* renderer, float x0, float y0, float width, float height, float angle, SDL_Color color)
{

	float borderLine = SDL_sqrtf((width / 2) * (width / 2) + height * height);
	float anglePoint = SDL_acosf(height / borderLine);

	SDL_FPoint peakPoint = { x0, y0 };
	SDL_FPoint bottomLeftPoint = { x0 + borderLine * SDL_sinf(-angle / 180.0 * 3.1415 + anglePoint), y0 + borderLine * SDL_cosf(-angle / 180.0 * 3.1415 + anglePoint) };
	SDL_FPoint bottomRightPoint = { x0 + borderLine * SDL_sinf(-angle / 180.0 * 3.1415 - anglePoint), y0 + borderLine * SDL_cosf(-angle / 180.0 * 3.1415 - anglePoint) };

	std::vector< SDL_Vertex > verts =
	{
		{ peakPoint, color, SDL_FPoint{ 0 }, },
		{ bottomLeftPoint, color, SDL_FPoint{ 0 }, },
		{ bottomRightPoint, color, SDL_FPoint{ 0 }, },
	};

	SDL_RenderGeometry(renderer, nullptr, verts.data(), verts.size(), nullptr, 0);
}