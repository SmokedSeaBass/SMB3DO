#pragma once

#include <SDL_rect.h>

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
