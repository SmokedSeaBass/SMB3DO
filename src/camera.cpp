#include "camera.h"

Camera::Camera(const SDL_Rect& rect) {
	rect_ = rect;
	target_ = CameraTarget();
	bounds_ = Rectangle(0, 0, -1, -1);
	capture_bounds_ = Rectangle(0, 0, 0, 0);
	vertical_scroll_ = VerticalScrollRule::ALWAYS;
	interp_ratio_ = 1;
}

std::vector<double> Camera::GetPosition() const {
	return { rect_.x, rect_.y };
}

std::vector<double> Camera::GetDimensions() const {
	return { rect_.w, rect_.h };
}

void Camera::SetRect(const Rectangle& rect) {
	rect_ = rect;
}

void Camera::SetPosition(double x, double y) {
	rect_.x = x;
	rect_.y = y;
}

void Camera::SetDimensions(double w, double h) {
	rect_.w = w;
	rect_.h = h;
}

void Camera::SetTarget(CameraTarget& target) {
	target_ = target;
}

void Camera::SetBounds(const Rectangle& rect) {
	bounds_ = rect;
}

void Camera::SetCaptureBounds(const Rectangle& rect) {
	capture_bounds_ = rect;
}

void Camera::SetVerticalScrollRule(VerticalScrollRule rule) {
	vertical_scroll_ = rule;
}

void Camera::SetInterpRatio(float interp_ratio) {
	if (interp_ratio < 0.0f)
		interp_ratio = 0.0f;
	if (interp_ratio > 1.0f)
		interp_ratio = 1.0f;
	interp_ratio_ = interp_ratio;
}

int Camera::Update(double delta_time) {
	if (target_.entity != nullptr) {
		target_.x = (int)(floor(target_.entity->GetPosition()[0]));
		target_.y = (int)(floor(target_.entity->GetPosition()[1]));
	}

	// Only move if target exits capture bounds
	Rectangle inner_bound = Rectangle(rect_.x + rect_.w / 2.0 + capture_bounds_.x , rect_.y + rect_.h / 2.0 + capture_bounds_.y, capture_bounds_.w, capture_bounds_.h);
	if (!capture_bounds_.w >= 0 && (target_.x < inner_bound.Left() || target_.x > inner_bound.Right())) {
		UpdatePosX(delta_time, inner_bound);
	}
	if (vertical_scroll_ != VerticalScrollRule::NONE && capture_bounds_.h >= 0 && (target_.y < inner_bound.Top() || target_.y > inner_bound.Bottom())) {
		UpdatePosY(delta_time, inner_bound);
	}
	return 0;
}

void Camera::UpdatePosX(double delta_time, const Rectangle& inner_bound) {
	if (interp_ratio_ == 1.0f) {
		rect_.x += inner_bound.DistanceToPoint(target_.x, target_.y)[0];
	} else {
		double dt_ratio = delta_time / (1000.0 / 60.0);
		double center_x = rect_.x + rect_.w / 2.0;
		double delta_x = inner_bound.DistanceToPoint(target_.x, target_.y)[0] * interp_ratio_ * dt_ratio;
		rect_.x += delta_x;
	}

	// Bound camera position
	if (bounds_.w > 0) {
		if (rect_.Left() < bounds_.Left()) {
			rect_.x = bounds_.Left();
		}
		if (rect_.Right() > bounds_.Right()) {
			rect_.x = bounds_.Right() - rect_.w;
		}
	}
}

void Camera::UpdatePosY(double delta_time, const Rectangle& inner_bound) {
	double delta_y = 0;
	switch (vertical_scroll_) {
	case (VerticalScrollRule::NONE):
		delta_y = 0;
		break;
	case (VerticalScrollRule::ALWAYS):
		delta_y = inner_bound.DistanceToPoint(target_.x, target_.y)[1];
		break;
	case (VerticalScrollRule::SELECTIVE):
		delta_y = std::max(0.0, inner_bound.DistanceToPoint(target_.x, target_.y)[1]);
		break;
	}
		
	if (interp_ratio_ == 1.0f) {
		rect_.y += delta_y;
	} else {
		double dt_ratio = delta_time / (1000.0 / 60.0);
		double center_y = rect_.y + rect_.y / 2.0;
		rect_.y += delta_y * interp_ratio_ * dt_ratio;
	}

	// Bound camera position
	if (bounds_.h > 0) {
		if (rect_.Top() < bounds_.Top()) {
			rect_.y = bounds_.Top();
		}
		if (rect_.Bottom() > bounds_.Bottom()) {
			rect_.y = bounds_.Bottom() - rect_.h;
		}
	}
}