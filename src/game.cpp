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

Game::Game() {
	Game::fps_limit = options.fps_limit;
	Game::fps_ratio = 60.0 / Game::fps_limit;
}

Game::~Game() { }

int Game::Run() {
	if (graphics.Initialize(options) < 0) {
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

	graphics.UpdateViewport(options);
	
	// Test creation code
	// Mario
	Sprite mario_spr = Sprite(graphics, "assets/sprite_sheets/mario_8bit.bmp", 0, 0, 215, 88, static_cast<int>(round(TILESIZE_NES)), static_cast<int>(round(TILESIZE_NES)));
	mario_spr.SetOrigin(Sprite::ORIGIN_ORIENTATION::BOTTOM_MIDDLE);
	Player mario = Player(&mario_spr, 32.0, 32.0);
	// Tileset
	Tileset debug_tileset = Tileset(graphics, "assets/tilesets/debug.tsx");
	// Tilemap
	Tilemap test_tilemap = Tilemap(graphics, "assets/maps/test.tmx");
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
				if (event.key.keysym.scancode == SDL_SCANCODE_F11 && !event.key.repeat) graphics.WindowToggleFullscreen(options);
				if (event.key.keysym.scancode == SDL_SCANCODE_P && !event.key.repeat) {
					options.TogglePixelRatio();
					graphics.UpdateViewport(options);
				};
				// TODO 6-21-21: Fix widescreen
				if (event.key.keysym.scancode == SDL_SCANCODE_O && !event.key.repeat) {
					options.enable_widescreen = !options.enable_widescreen;
					options.forceIntegerScaling = false;
					graphics.UpdateViewport(options);
				};
				if (event.key.keysym.scancode == SDL_SCANCODE_I && !event.key.repeat) {
					options.forceIntegerScaling = !options.forceIntegerScaling;
					options.enable_widescreen = false;
					graphics.UpdateViewport(options);
				};
			}
			if (event.type == SDL_WINDOWEVENT) {
				if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
					graphics.UpdateViewport(options);
				}
			}
		}
		input.UpdateInputs(keyboard_state);
		
		/* Update */
		delta_time = tick_duration + tick_wait;

		mario.Update(input, delta_time, test_tilemap);
		debug_tileset.Update(delta_time);

		std::vector<Tilemap::CollisionTile> colliding_tiles = test_tilemap.GetCollidingTiles(mario.GetColliderAbsoluteRect());
		printf("[");
		for (Tilemap::CollisionTile t : colliding_tiles) {
			printf("%i ", t.tile.GetCollision());
		}
		printf("]\n");

		/* Draw */
		// Draw tilemap
		test_tilemap.Draw(graphics, 0, 0);// , { (float)mario.GetColliderAbsoluteRect().x, (float)mario.GetColliderAbsoluteRect().y, (float)mario.GetColliderAbsoluteRect().w, (float)mario.GetColliderAbsoluteRect().h });
		// Draw objects
		/*SDL_Rect block = { ceil((double)16.1), (double)128, ceil((double)16), (double)16 };
		graphics.BlitColoredRect(&block, 0x10, 0x10, 0x40, 0xFF);*/
		// Draw player
		mario.Draw(graphics);
		// Flip to screen
		graphics.FlipRenderer();

		// Frames-per-second management
		const std::chrono::high_resolution_clock::time_point tick_end = std::chrono::high_resolution_clock::now();
		tick_duration = std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(tick_end - tick_start).count();
		if (Game::fps_limit > 0)
			tick_wait = 1000.0 / Game::fps_limit - tick_duration;
		if (tick_wait > 0)
			SDL_Delay(static_cast<Uint32>(tick_wait));
		// Add to window's title
		char title_buff[48];
		snprintf(title_buff, sizeof(title_buff), "%.0f/%.2f fps (%.4f/%.4f ms)", 1000.0 / (tick_duration + tick_wait), 1000.0 / tick_duration, tick_duration + tick_wait, tick_duration);
		graphics.WindowSetTitle(title_buff);
	}
	return 0;
}