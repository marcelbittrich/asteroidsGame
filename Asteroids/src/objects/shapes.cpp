#include "shapes.hpp"

#ifndef PI
	#define PI 3.14159265359f
#endif


//mid point circle algorithm based on https://en.wikipedia.org/w/index.php?title=Midpoint_circle_algorithm&oldid=889172082#C_example
void DrawCircle(SDL_Renderer* renderer, const Vec2& position, const SDL_Color& color, int radius)
{
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

	float x0 = position.x;
	float y0 = position.y;
	float x = (float)radius - 1;
	float y = 0;
	float dx = 1;
	float dy = 1;
	float err = dx - (radius << 1);

	std::vector<SDL_FPoint> points;

	while (x >= y)
	{
		points.push_back(SDL_FPoint{ x0 + x, y0 + y });
		points.push_back(SDL_FPoint{ x0 + y, y0 + x });
		points.push_back(SDL_FPoint{ x0 - y, y0 + x });
		points.push_back(SDL_FPoint{ x0 - x, y0 + y });
		points.push_back(SDL_FPoint{ x0 - x, y0 - y });
		points.push_back(SDL_FPoint{ x0 - y, y0 - x });
		points.push_back(SDL_FPoint{ x0 + y, y0 - x });
		points.push_back(SDL_FPoint{ x0 + x, y0 - y });

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

	SDL_RenderDrawPointsF(renderer, &points[0], (int)points.size());
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