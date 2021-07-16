#pragma once

#include <memory>
#include "entity.h"
#include "rectangle.h"

class Camera {
public:
	struct CameraTarget {
		int x, y;
		const Entity* entity;

		CameraTarget() : x(0), y(0), entity(nullptr) {}
		CameraTarget(const Entity& entity) : x(entity.GetPosition()[0]), y(entity.GetPosition()[1]), entity(&entity) { }
		CameraTarget(int x, int y) : x(x), y(y), entity(nullptr) { }
	};

	Camera(const SDL_Rect& rect = { 0, 0, WINDOW_WIDTH_NES, WINDOW_HEIGHT_NES });
	
	void SetRect(const Rectangle& rect);
	void SetPosition(double x, double y);
	void SetDimensions(double w, double h);
	void SetTarget(CameraTarget& target);
	void SetInterpRatio(float interp_ratio);
	std::vector<double> GetPosition() const;
	int Update(double delta_time);

private:
	Rectangle rect_;
	CameraTarget* target_;
	float interp_ratio_;
};

