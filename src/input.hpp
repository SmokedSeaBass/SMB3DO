#pragma once

#include <map>
#include <vector>
#include "constants.h"

class Input {
public:
	enum class Button {
		P1_UP,
		P1_LEFT,
		P1_DOWN,
		P1_RIGHT,
		P1_A,
		P1_B,
		P1_START,
		P1_SELECT
	};
	enum class Mode {
		KEYBOARD,
		CONTROLLER
	};
	std::map<Button, bool> input_map_;
	std::map<Button, bool> input_map_prev_;

	Input(Mode mode = Mode::KEYBOARD);
	~Input();

	void UpdateInputs(const Uint8* keyboard_state);
	bool IsButtonPressed(const Button& button) const;  // Only true on the frame a button was pressed
	bool IsButtonDown(const Button& button) const;
	bool IsButtonReleased(const Button& button) const;  // Only true on the frame a button was released
	bool IsButtonUp(const Button& button) const;

private:
	Mode input_mode_;
	std::vector<Button> button_vector_;
};
