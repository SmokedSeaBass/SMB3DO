#pragma once
#if defined(_WIN32)
	#include <SDL.h>
#elif __linux__
	#include <SDL2/SDL.h>
#endif
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
	bool IsButtonPressed(Button button);  // Only true on the frame a button was pressed
	bool IsButtonDown(Button button);
	bool IsButtonReleased(Button button);  // Only true on the frame a button was released
	bool IsButtonUp(Button button);

private:
	Mode input_mode_;
	std::vector<Button> button_vector_;
};
