#pragma once


class Rectangle {
public:
	Rectangle() :
		x(0), y(0), w(0), h(0) {}
	Rectangle(double x, double y, double w, double h) :
		x(x), y(y), w(w), h(h) {}
	Rectangle(SDL_Rect sdl_rect) :
		x(sdl_rect.x), y(sdl_rect.y), w(sdl_rect.w), h(sdl_rect.h) {}

	double Left() const { return x; }
	double Right() const { return x + w; }
	double Top() const { return y; }
	double Bottom() const { return y + h; }

	double x, y, w, h;
};