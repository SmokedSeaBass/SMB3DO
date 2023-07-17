#pragma once

#if defined(_WIN32)
	#include <SDL.h>
#elif __linux__
	#include <SDL2/SDL.h>
#endif

class Rectangle {
public:
	Rectangle() :
		x(0), y(0), w(0), h(0) {}
	Rectangle(double x, double y, double w, double h) :
		x(x), y(y), w(w), h(h) {}
	Rectangle(SDL_Rect sdl_rect) :
		x(static_cast<double>(sdl_rect.x)), y(static_cast<double>(sdl_rect.y)), w(static_cast<double>(sdl_rect.w)), h(static_cast<double>(sdl_rect.h)) {}

	double Left() const { return x; }
	double Right() const { return x + w; }
	double Top() const { return y; }
	double Bottom() const { return y + h; }

	bool ContainsPoint(double x, double y) const { return x >= Left() && x <= Right() && y >= Top() && y <= Bottom(); }
	std::vector<double> DistanceToPoint(double x, double y) const {
		double delta_x = 0;
		double delta_y = 0;
		if (ContainsPoint(x, y)) {
			return { delta_x, delta_y };
		}
		if (x < Left()) {
			delta_x = x - Left();
		} else {
			delta_x = x - Right();
		}
		if (y < Top()) {
			delta_y = y - Top();
		} else {
			delta_y = y - Bottom();
		}
		return { delta_x, delta_y };
	}

	Rectangle operator+(const Rectangle& other) const {
		return Rectangle(
			this->x + other.x,
			this->y + other.y,
			this->w + other.w,
			this->h + other.h
		);
	}

	double x, y, w, h;
};
