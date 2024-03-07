#include "shapes.hpp"

#ifndef PI
	#define PI 3.14159265359f
#endif


//mid point circle algorithm based on https://en.wikipedia.org/w/index.php?title=Midpoint_circle_algorithm&oldid=889172082#C_example
void DrawCircle(SDL_Renderer* renderer, const Vec2& position, const SDL_Color& color, int radius)
{
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

	int x0 = (int)position.x;
	int y0 = (int)position.y;
	int x = radius - 1;
	int y = 0;
	int dx = 1;
	int dy = 1;
	int err = dx - (radius << 1);

	while (x >= y)
	{
		SDL_Point points[8] = {};

		points[0] = {x0 + x, y0 + y};
		points[1] = { x0 + y, y0 + x };
		points[2] = { x0 - y, y0 + x };
		points[3] = { x0 - x, y0 + y };
		points[4] = { x0 - x, y0 - y };
		points[5] = { x0 - y, y0 - x };
		points[6] = { x0 + y, y0 - x };
		points[7] = { x0 + x, y0 - y };

		SDL_RenderDrawPoints(renderer, points, 8);

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

void DrawTriangle(SDL_Renderer* renderer, const Vec2& position, float width, float height, float rotation, SDL_Color color)
{
	float borderLine = SDL_sqrtf((width / 2) * (width / 2) + height * height);
	float anglePoint = SDL_acosf(height / borderLine);

	SDL_FPoint peakPoint = { position.x, position.y };
	SDL_FPoint bottomLeftPoint = { position.x + borderLine * SDL_sinf(-rotation / 180.f * (float)PI + anglePoint), position.y + borderLine * SDL_cosf(-rotation / 180.f * (float)PI + anglePoint) };
	SDL_FPoint bottomRightPoint = { position.x + borderLine * SDL_sinf(-rotation / 180.f * (float)PI - anglePoint), position.y + borderLine * SDL_cosf(-rotation / 180.f * (float)PI - anglePoint) };

	std::vector< SDL_Vertex > verts =
	{
		{ peakPoint, color, SDL_FPoint{ 0 }, },
		{ bottomLeftPoint, color, SDL_FPoint{ 0 }, },
		{ bottomRightPoint, color, SDL_FPoint{ 0 }, },
	};

	SDL_RenderGeometry(renderer, nullptr, verts.data(), (int)verts.size(), nullptr, 0);
}