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

	enum class VerticalScrollRule {
		NONE, ALWAYS, SELECTIVE
	};

	Camera(const SDL_Rect& rect = { 0, 0, (int)NES_WINDOW_WIDTH, (int)NES_WINDOW_HEIGHT });

	std::vector<double> GetPosition() const;
	std::vector<double> GetDimensions() const;
	
	void SetRect(const Rectangle& rect);
	void SetPosition(double x, double y);
	void SetDimensions(double w, double h);
	void SetTarget(const CameraTarget& target);
	void SetBounds(const Rectangle& rect);
	void SetCaptureBounds(const Rectangle& rect);
	void SetVerticalScrollRule(VerticalScrollRule rule);
	void SetInterpRatio(float interp_ratio);
	int Update(double delta_time);

private:
	Rectangle rect_;
	Rectangle bounds_;
	Rectangle capture_bounds_;
	CameraTarget target_;
	VerticalScrollRule vertical_scroll_;
	float interp_ratio_;

	void UpdatePosX(double delta_time, const Rectangle& inner_bound);
	void UpdatePosY(double delta_time, const Rectangle& inner_bound);
};

