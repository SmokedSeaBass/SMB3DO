#include "game.h"

#include <chrono>
#include "animated_sprite.h"
#include "bitmap_font.h"
#include "camera.h"
#include "constants.h"
#include "error.h"
#include "input.h"
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
			options_.interp_factor = static_cast<int>(ceil(Game::fps_limit / 60.0));
		} else {
			options_.interp_factor = static_cast<int>(ceil(graphics_.GetCurrentDisplayMode().refresh_rate / 60.0));
		}
	}
	double delta_time = 1000.0 / (60.0 * options_.interp_factor);
	std::chrono::high_resolution_clock::time_point tick_start = std::chrono::high_resolution_clock::now();
	
	// Test creation code
	// Mario
	Player mario = Player(graphics_, "assets/sprite_sheets/mario.bmp", 32, 440);
	// Tileset
	//Tileset debug_tileset(graphics_, "assets/tilesets/debug.tsx");
	// Tilemap
	Tilemap test_tilemap = Tilemap(graphics_, "assets/maps/test2.tmx");
	//test_tilemap.AddTileset(&debug_tileset);

	// Bitmap font
	SDL_Texture* test_font_texture = graphics_.LoadTextureFromImage("assets/sprite_sheets/hud_font_ascii.bmp", 0, 0);
	BitmapFont test_font;
	test_font.LoadBitmap(test_font_texture, 8, 8);
	// Camera
	Camera camera(graphics_.GetViewport());
	camera.SetTarget(Camera::CameraTarget(mario));
	camera.SetPosition(0, 12 * 16);
	camera.SetBounds(Rectangle(0, 0, test_tilemap.GetDimensions()[0] * TILESIZE_NES, test_tilemap.GetDimensions()[1] * TILESIZE_NES));
	camera.SetCaptureBounds(Rectangle(-8, 0, 16, 0));
	camera.SetVerticalScrollRule(Camera::VerticalScrollRule::SELECTIVE);
	camera.SetInterpRatio(1.0f);
	
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
					};
					if (event.key.keysym.scancode == SDL_SCANCODE_O && !event.key.repeat) {
						options_.enable_widescreen = !options_.enable_widescreen;
						options_.forceIntegerScaling = false;
						graphics_.UpdateViewport(options_);
					};
					if (event.key.keysym.scancode == SDL_SCANCODE_I && !event.key.repeat) {
						options_.forceIntegerScaling = !options_.forceIntegerScaling;
						options_.enable_widescreen = false;
						graphics_.UpdateViewport(options_);
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
					}
				}
			}
			input.UpdateInputs(keyboard_state);

			/* Update */
			mario.Update(input, delta_time, test_tilemap);
			camera.SetDimensions(graphics_.GetViewport().w, graphics_.GetViewport().h);
			camera.Update(delta_time);
			test_tilemap.Update(delta_time);

			tick_accumulator -= delta_time;
		}

		/* Draw */
		// Backdraw color
		SDL_Rect game_view = graphics_.GetViewport();
		graphics_.SetViewport(NULL);
		graphics_.DrawColoredRect(NULL, 0x00, 0x00, 0x00, 0xFF);
		graphics_.SetViewport(&game_view);
		graphics_.DrawColoredRect(NULL, 0xAF, 0xE5, 0xEA, 0xFF);
		// Draw tilemap
		test_tilemap.Draw(graphics_, -static_cast<int>(floor(camera.GetPosition()[0])), -static_cast<int>(floor(camera.GetPosition()[1])));
		// Draw objects
		// Draw player
		mario.Draw(graphics_, -static_cast<int>(floor(camera.GetPosition()[0])), -static_cast<int>(floor(camera.GetPosition()[1])));
		// UI
		if (Game::debug_show_info) {
			char debug_info[48] = "";
			if (Game::fps_limit > 0) {
				snprintf(debug_info, sizeof(debug_info), "%.2f/%.0f fps\n%.4f/%.2f ms", 1000.0 / (tick_duration), Game::fps_limit, tick_duration, 1000.0 / Game::fps_limit);
			} else {
				snprintf(debug_info, sizeof(debug_info), "%.2f fps\n%.4f ms", 1000.0 / (tick_duration), tick_duration);
			}
			test_font.DrawText(graphics_, debug_info, 0, 0);
			test_font.DrawText(graphics_, "X: " + std::to_string(mario.GetPosition()[0]), 0, 16);
			test_font.DrawText(graphics_, "Y: " + std::to_string(mario.GetPosition()[1]), 0, 24);
			test_font.DrawText(graphics_, "CX: " + std::to_string(camera.GetPosition()[0]), 0, 32);
			test_font.DrawText(graphics_, "CY: " + std::to_string(camera.GetPosition()[1]), 0, 40);
		}
		// Flip to screen
		graphics_.FlipRenderer();

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
