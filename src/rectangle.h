#pragma once


class Rectangle {
public:
	Rectangle() :
		x_(0), y_(0), w_(0), h_(0) {}
	Rectangle(double x, double y, double w, double h) :
		x_(x), y_(y), w_(w), h_(h) {}
	Rectangle(SDL_Rect sdl_rect) :
		x_(sdl_rect.x), y_(sdl_rect.y), w_(sdl_rect.w), h_(sdl_rect.h) {}

	double Left() const { return x_; }
	double Right() const { return x_ + w_; }
	double Top() const { return y_; }
	double Bottom() const { return y_ + h_; }

	double Width() const { return w_; }
	double Height() const { return h_; }

private:
	double x_, y_, w_, h_;
};