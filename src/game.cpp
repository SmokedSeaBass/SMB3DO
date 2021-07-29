#include "game.h"

#include <chrono>
#include "animated_sprite.h"
//#include "bitmap_font.h"
#include "camera.h"
#include "constants.h"
#include "error.h"
#include "input.h"
#include "level.h"
#include "player.h"
#include "sprite.h"
#include "tilemap.h"

double Game::fps_limit = 0;
float Game::time_multiplier = 1;	// Unused

bool Game::debug_show_hitboxes = false;
bool Game::debug_show_info = false;

Game::Game() {
	if (!options_.enable_vsync) {
		Game::fps_limit = options_.fps_limit;
	}
	if (META_DEBUG) {
		Game::debug_show_hitboxes = true;
		Game::debug_show_info = true;
	}
}

Game::~Game() { }

int Game::GetInput(Input& input, SDL_Event& event, const Uint8* keyboard_state, bool& quit_game) {
	return 0;
}

int Game::Update(const double delta_time) {
	return 0;
}

int Game::Render(Graphics& graphics) {
	return 0;
}

int Game::Run() {
	if (graphics_.Initialize(options_) < 0) {
		Error::PrintError(std::runtime_error("Could not initialize graphics"));
		return -1;
	}
	Input input;
	bool quit_game = false;
	SDL_Event event;
	const Uint8* keyboard_state = SDL_GetKeyboardState(nullptr);

	if (options_.enable_vsync) {
		Game::fps_limit = graphics_.GetCurrentDisplayMode().refresh_rate;
	}
	double tick_accumulator = 0.0;
	if (options_.interp_factor == 0) {
		if (fps_limit != 0) {
			options_.interp_factor = (int)ceil(Game::fps_limit / 60.0);
		} else {
			options_.interp_factor = (int)ceil(graphics_.GetCurrentDisplayMode().refresh_rate / 60.0);
		}
	}
	double delta_time = 1000.0 / (60.0 * options_.interp_factor);
	std::chrono::high_resolution_clock::time_point tick_start = std::chrono::high_resolution_clock::now();
	
	// Test creation code
	Level test_level;
	test_level.Load(graphics_, "assets/maps/test2a.tmx");

	// Bitmap font
	graphics_.LoadBMPFont("assets/sprite_sheets/hud_font_ascii.bmp", 8, 8, "hud");
	graphics_.LoadBMPFont("assets/sprite_sheets/dialogue_font_ascii.bmp", 8, 8, "dialogue");
	
	// Main game loop
	while (!quit_game) {
		const std::chrono::high_resolution_clock::time_point tick_start_new = std::chrono::high_resolution_clock::now();
		const double tick_duration = std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(tick_start_new - tick_start).count();
		tick_accumulator += std::min(tick_duration, 1000.0 / 30.0);		// Force minimum 30 fps, enables slowdown if below
		tick_start = tick_start_new;

		while (tick_accumulator >= delta_time) {
			/* Get Input */
			while (SDL_PollEvent(&event) != 0) {
				if (event.type == SDL_QUIT) quit_game = true;
				if (event.type == SDL_KEYDOWN) {
					if (event.key.keysym.scancode == SDL_SCANCODE_END) return -1;		// Force crash
					if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) quit_game = true;
					if (event.key.keysym.scancode == SDL_SCANCODE_F11 && !event.key.repeat) graphics_.WindowToggleFullscreen(options_);
					if (event.key.keysym.scancode == SDL_SCANCODE_P && !event.key.repeat) {
						options_.TogglePixelRatio();
						graphics_.UpdateViewport(options_);
						graphics_.UpdateCanvas(options_);
					};
					if (event.key.keysym.scancode == SDL_SCANCODE_O && !event.key.repeat) {
						options_.enable_widescreen = !options_.enable_widescreen;
						options_.forceIntegerScaling = false;
						graphics_.UpdateViewport(options_);
						graphics_.UpdateCanvas(options_);
					};
					if (event.key.keysym.scancode == SDL_SCANCODE_I && !event.key.repeat) {
						options_.forceIntegerScaling = !options_.forceIntegerScaling;
						options_.enable_widescreen = false;
						graphics_.UpdateViewport(options_);
						graphics_.UpdateCanvas(options_);
					};
					if (event.key.keysym.scancode == SDL_SCANCODE_H && !event.key.repeat) {
						Game::debug_show_hitboxes = !Game::debug_show_hitboxes;
					};
					if (event.key.keysym.scancode == SDL_SCANCODE_U && !event.key.repeat) {
						Game::debug_show_info = !Game::debug_show_info;
					};
				}
				if (event.type == SDL_WINDOWEVENT) {
					if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
						graphics_.UpdateViewport(options_);
						graphics_.UpdateCanvas(options_);
					}
				}
			}
			input.UpdateInputs(keyboard_state);

			/* Update */
			test_level.Update(graphics_, input, delta_time);

			tick_accumulator -= delta_time;
		}

		/* Draw */
		// Backdraw color
		//SDL_Rect game_view = graphics_.GetViewport();
		//graphics_.SetViewport(NULL);
		//graphics_.DrawColoredRect(NULL, 0x00, 0x00, 0x00, 0xFF);
		//graphics_.SetViewport(&game_view);
		
		// Level
		test_level.Draw(graphics_);

		// UI
		if (Game::debug_show_info) {
			char debug_info[48] = "";
			if (Game::fps_limit > 0) {
				snprintf(debug_info, sizeof(debug_info), "%.2f/%.0f fps\n%.4f/%.2f ms", 1000.0 / (tick_duration), Game::fps_limit, tick_duration, 1000.0 / Game::fps_limit);
			} else {
				snprintf(debug_info, sizeof(debug_info), "%.2f fps\n%.4f ms", 1000.0 / (tick_duration), tick_duration);
			}
			graphics_.SetTextFont("hud");
			graphics_.DrawText(debug_info, 0, 0);
		}
		// Flip to screen
		graphics_.PresentRender();

		// Framerate limiter
		if (!options_.enable_vsync) {
			const std::chrono::high_resolution_clock::time_point tick_end = std::chrono::high_resolution_clock::now();
			double frame_time = std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(tick_start_new - tick_end).count();
			double tick_wait = 0.0;
			if (Game::fps_limit > 0)
				tick_wait = (1000.0 / (Game::fps_limit)) - frame_time;
			if (tick_wait > 0) {
				SDL_Delay(static_cast<Uint32>(tick_wait));
			}
		}
	}
	return 0;
}
