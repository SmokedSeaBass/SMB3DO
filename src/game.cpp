#include "game.h"

#include <chrono>
#include "constants.h"
#include "tilemap.h"
#include "sprite.h"
#include "animated_sprite.h"
#include "player.h"
#include "input.h"
#include "error.h"

double Game::fps_limit = 0;
double Game::fps_ratio = 0;
float Game::time_multiplier = 0;

bool Game::debug_show_hitboxes = false;

Game::Game() {
	Game::fps_limit = options_.fps_limit;
	Game::fps_ratio = 60.0 / Game::fps_limit;		// TODO: Replace uses with proper delta_time math
	Game::time_multiplier = 1;
	if (META_DEBUG) {
		Game::debug_show_hitboxes = true;
	}
}

Game::~Game() { }

int Game::Run() {
	if (graphics_.Initialize(options_) < 0) {
		Error::PrintError(std::runtime_error("Could not initialize graphics"));
		return -1;
	}
	Input input;
	bool quitGame = false;
	SDL_Event event;
	const Uint8* keyboard_state = SDL_GetKeyboardState(nullptr);

	double tick_duration = 0;
	double tick_wait = 0;
	double delta_time = 0;

	graphics_.UpdateViewport(options_);
	
	// Test creation code
	// Mario
	Sprite mario_spr = Sprite(graphics_, "assets/sprite_sheets/mario_8bit.bmp", 0, 0, 215, 88, static_cast<int>(round(TILESIZE_NES)), static_cast<int>(round(TILESIZE_NES)));
	mario_spr.SetOrigin(Sprite::ORIGIN_ORIENTATION::BOTTOM_MIDDLE);
	Player mario = Player(&mario_spr, 32.0, 32.0);
	// Tileset
	Tileset debug_tileset = Tileset(graphics_, "assets/tilesets/debug.tsx");
	// Tilemap
	Tilemap test_tilemap = Tilemap(graphics_, "assets/maps/test.tmx");
	test_tilemap.SetTileset(&debug_tileset);
	
	// Main game loop
	while (!quitGame) {
		const std::chrono::high_resolution_clock::time_point tick_start = std::chrono::high_resolution_clock::now();

		/* Get Input */
		// Handle events
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
		// Flip to screen
		graphics_.FlipRenderer();

		// Frames-per-second management
		const std::chrono::high_resolution_clock::time_point tick_end = std::chrono::high_resolution_clock::now();
		tick_duration = std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(tick_end - tick_start).count();
		if (Game::fps_limit > 0)
			tick_wait =  (1000.0 / (Game::fps_limit * Game::time_multiplier))  - tick_duration;
		delta_time = tick_duration + std::max(tick_wait, 0.0);
		if (tick_wait > 0)
			SDL_Delay(static_cast<Uint32>(tick_wait));
		// Add to window's title
		char title_buff[48];
		snprintf(title_buff, sizeof(title_buff), "%.2f/%.0f fps (%.4f/%.4f ms)", 1000.0 / (tick_duration), Game::fps_limit, tick_duration, 1000.0 / Game::fps_limit);
		graphics_.WindowSetTitle(title_buff);
	}
	return 0;
}