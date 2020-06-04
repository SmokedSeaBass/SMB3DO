#include "input.h"

Input::Input(Mode mode) {
	input_mode_ = mode;
	button_vector_ = {
		Button::P1_UP,
		Button::P1_LEFT,
		Button::P1_DOWN,
		Button::P1_RIGHT,
		Button::P1_A,
		Button::P1_B,
		Button::P1_START,
		Button::P1_SELECT
	};
	for (int i = 0; i < button_vector_.size(); i++) {
		input_map_[button_vector_[i]] = false;
		input_map_prev_[button_vector_[i]] = false;
	}
}

Input::~Input() { }

void Input::UpdateInputs(const Uint8* keyboard_state) {
	input_map_prev_ = input_map_;
	if (input_mode_ == Mode::KEYBOARD) {
		input_map_[Button::P1_UP] = keyboard_state[SDL_SCANCODE_W];
		input_map_[Button::P1_LEFT] = keyboard_state[SDL_SCANCODE_A];
		input_map_[Button::P1_DOWN] = keyboard_state[SDL_SCANCODE_S];
		input_map_[Button::P1_RIGHT] = keyboard_state[SDL_SCANCODE_D];
		input_map_[Button::P1_A] = keyboard_state[SDL_SCANCODE_L];
		input_map_[Button::P1_B] = keyboard_state[SDL_SCANCODE_K];
		input_map_[Button::P1_START] = keyboard_state[SDL_SCANCODE_RETURN];
		input_map_[Button::P1_SELECT] = keyboard_state[SDL_SCANCODE_SPACE];
	} else if (input_mode_ == Mode::CONTROLLER) {

	}
}

bool Input::IsButtonPressed(Button button) {
	if (input_map_[button] && !input_map_prev_[button]) return true;
	return false;
}

bool Input::IsButtonDown(Button button) {
	return input_map_[button];
}

bool Input::IsButtonReleased(Button button) {
	if (!input_map_[button] && input_map_prev_[button]) return true;
	return false;
}

bool Input::IsButtonUp(Button button) {
	return !input_map_[button];
}