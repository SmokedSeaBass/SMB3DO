#include "game.h"

#include <chrono>
#include "animated_sprite.h"
#include "bitmap_font.h"
#include "constants.h"
#include "error.h"
#include "input.h"
#include "player.h"
#include "sprite.h"
#include "tilemap.h"

double Game::fps_limit = 0;
double Game::fps_ratio = 0;
float Game::time_multiplier = 1;

bool Game::debug_show_hitboxes = false;
bool Game::debug_show_info = false;

Game::Game() {
	if (!options_.enable_vsync) {
		Game::fps_limit = options_.fps_limit;
	}
	Game::time_multiplier = 1;
	if (META_DEBUG) {
		Game::debug_show_hitboxes = true;
		Game::debug_show_info = true;
	}
}

Game::~Game() { }

int Game::GetInput() {
	return 0;
}

int Game::Update() {
	return 0;
}

int Game::Render() {
	return 0;
}

int Game::Run() {
	if (graphics_.Initialize(options_) < 0) {
		Error::PrintError(std::runtime_error("Could not initialize graphics"));
		return -1;
	}
	Input input;
	bool quitGame = false;
	SDL_Event event;
	const Uint8* keyboard_state = SDL_GetKeyboardState(nullptr);

	if (options_.enable_vsync) {
		Game::fps_limit = graphics_.GetCurrentDisplayMode().refresh_rate;
	}
	double tick_accumulator = 0.0;
	double delta_time = 1000.0 / 60.0;
	if (options_.enable_vsync && Game::fps_limit != 0) {
		delta_time = 1000.0 / (60.0 * ceil(Game::fps_limit / 60.0));
	}
	std::chrono::high_resolution_clock::time_point tick_start = std::chrono::high_resolution_clock::now();

	graphics_.UpdateViewport(options_);
	
	// Test creation code
	// Mario
	Player mario = Player(graphics_, "assets/sprite_sheets/mario.bmp", 32, 32);
	// Tileset
	Tileset debug_tileset = Tileset(graphics_, "assets/tilesets/debug.tsx");
	// Tilemap
	Tilemap test_tilemap = Tilemap(graphics_, "assets/maps/test.tmx");
	test_tilemap.SetTileset(&debug_tileset);

	SDL_Texture* test_font_texture = graphics_.LoadTextureFromImage("assets/sprite_sheets/hud_font_ascii.bmp", 0, 0);
	BitmapFont test_font;
	test_font.LoadBitmap(test_font_texture, 8, 8);
	
	// Main game loop
	while (!quitGame) {
		const std::chrono::high_resolution_clock::time_point tick_start_new = std::chrono::high_resolution_clock::now();
		const double tick_duration = std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(tick_start_new - tick_start).count();
		tick_accumulator += std::min(tick_duration, 1000.0 / 30.0);		// Force minimum 30 fps, enables slowdown if below
		tick_start = tick_start_new;

		while (tick_accumulator >= delta_time) {
			/* Get Input */
			while (SDL_PollEvent(&event) != 0) {
				if (event.type == SDL_QUIT) quitGame = true;
				if (event.type == SDL_KEYDOWN) {
					if (event.key.keysym.scancode == SDL_SCANCODE_END) return -1;		// Force crash
					if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) quitGame = true;
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
			debug_tileset.Update(delta_time);

			tick_accumulator -= delta_time;
		}

		/*std::vector<Tilemap::CollisionTile> colliding_tiles = test_tilemap.GetCollidingTiles(mario.GetColliderAbsoluteRect());
		printf("[");
		for (Tilemap::CollisionTile t : colliding_tiles) {
			printf("%i ", t.tile.GetCollision());
		}
		printf("]\n");*/

		/* Draw */
		// Draw tilemap
		test_tilemap.Draw(graphics_, 0, 0);// , { (float)mario.GetColliderAbsoluteRect().x, (float)mario.GetColliderAbsoluteRect().y, (float)mario.GetColliderAbsoluteRect().w, (float)mario.GetColliderAbsoluteRect().h });
		// Draw objects
		//SDL_Rect block = { (double)16, (double)128, (double)16, (double)16 };
		//graphics.DrawColoredOutline({block.x, block.y}, {block.x + block.w, block.y + block.h} , 0x10, 0x10, 0x40, 0xFF);
		//graphics.DrawColoredOutline(Rectangle(32.5, 128, 16, 16), 0x80, 0x10, 0x10, 0xFF);
		// Draw player
		mario.Draw(graphics_);
		// UI
		if (Game::debug_show_info) {
			char debug_info[48] = "";
			if (Game::fps_limit > 0) {
				snprintf(debug_info, sizeof(debug_info), "%.2f/%.0f fps\n%.4f/%.2f ms", 1000.0 / (tick_duration), Game::fps_limit, tick_duration, 1000.0 / Game::fps_limit);
			} else {
				snprintf(debug_info, sizeof(debug_info), "%.2f fps\n%.4f ms", 1000.0 / (tick_duration), tick_duration);
			}
			test_font.DrawText(graphics_, debug_info, 0, 0);
		}
		// Flip to screen
		graphics_.FlipRenderer();

		// Framerate limiter
		if (!options_.enable_vsync) {
			const std::chrono::high_resolution_clock::time_point tick_end = std::chrono::high_resolution_clock::now();
			double frame_time = std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(tick_start_new - tick_end).count();
			double tick_wait = 0.0;
			if (Game::fps_limit > 0)
				tick_wait = (1000.0 / (Game::fps_limit * Game::time_multiplier)) - frame_time;
			if (tick_wait > 0) {
				SDL_Delay(static_cast<Uint32>(tick_wait));
			}
		}
	}
	return 0;
}