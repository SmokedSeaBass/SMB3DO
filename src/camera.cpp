#include "camera.h"

Camera::Camera(const SDL_Rect& rect) {
	rect_ = rect;
	target_ = nullptr;
	interp_ratio_ = 1;
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
	target_ = &target;
}

void Camera::SetInterpRatio(float interp_ratio) {
	if (interp_ratio < 0.0f)
		interp_ratio = 0.0f;
	if (interp_ratio > 1.0f)
		interp_ratio = 1.0f;
	interp_ratio_ = interp_ratio;
}

std::vector<double> Camera::GetPosition() const {
	return std::vector<double>({ static_cast<double>(rect_.x), static_cast<double>(rect_.y) });
}

int Camera::Update(double delta_time) {
	if (target_ == nullptr) {
		return 0;
	}
	if (target_->entity != nullptr) {
		target_->x = static_cast<int>(floor(target_->entity->GetPosition()[0]));
		target_->y = static_cast<int>(floor(target_->entity->GetPosition()[1]));
	}
	if (interp_ratio_ == 1.0f) {
		rect_.x = (double)target_->x - rect_.w / 2.0;
		rect_.y = (double)target_->y - rect_.h / 2.0;
		return 0;
	}
	double dt_ratio = delta_time / (1000.0 / 60.0);
	double center_x = rect_.x + rect_.w / 2.0;
	double center_y = rect_.y + rect_.h / 2.0;
	double delta_x = ((double)target_->x - center_x) * interp_ratio_ * dt_ratio;
	double delta_y = ((double)target_->y - center_y) * interp_ratio_ * dt_ratio;
	if (abs(delta_x) < 0.1 * interp_ratio_ * dt_ratio) {
		rect_.x = (double)target_->x - rect_.w / 2.0;
	} else {
		rect_.x += delta_x;
	}
	if (abs(delta_y) < 0.1 * interp_ratio_ * dt_ratio) {
		rect_.y = (double)target_->y - rect_.h / 2.0;
	} else {
		rect_.y += delta_y;
	}
	return 0;
}