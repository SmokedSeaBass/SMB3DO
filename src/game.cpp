#include "game.h"

#include <chrono>
#include "constants.h"
#include "tilemap.h"
#include "sprite.h"
#include "player.h"
#include "input.h"
#include "error.h"

Game::Game() { }

Game::~Game() { }

int Game::Run() {
	Graphics graphics;
	if (graphics.Initialize() < 0) {
		Error::PrintError(std::runtime_error("Graphics could not initialize"));
		return -1;
	}
	Input input;
	bool quitGame = false;
	SDL_Event event;
	const Uint8* keyboard_state = SDL_GetKeyboardState(nullptr);
	
	// Test creation code
	// Mario
	Sprite mario_spr = Sprite(graphics, "assets/sprite_sheets/mario_8bit.bmp", 0, 0, 215, 88, static_cast<int>(round(BLOCKSIZE_NES)), static_cast<int>(round(BLOCKSIZE_NES)));
	mario_spr.SetOrigin(Sprite::ORIGIN_ORIENTATION::BOTTOM_MIDDLE);
	Player mario = Player(&mario_spr, 32.0, 32.0);
	// Tileset
	Sprite tileset_debug = Sprite(graphics, "assets/tilesets/debug.bmp");
	Tile air = Tile(0x00, &tileset_debug, { 0, 0, 16, 16 }, "air");
	Tile wood = Tile(0x01, &tileset_debug, { 0, 16, 16, 16 }, "wood");
	Tile* background_tiles[30] = {
		nullptr, &air, &air, &air, nullptr,
		&air, &wood, &air, &wood, &air,
		&air, &air, &air, &air, &air,
		&wood, &air, &air, &air, &wood,
		&air, &wood, &wood, &wood, &air,
		nullptr, &air, &air, &air, nullptr
	};
	Tilemap background_tilemap = Tilemap(background_tiles, 5, 6);
	
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
				if (event.key.keysym.scancode == SDL_SCANCODE_F11) graphics.WindowToggleFullscreen();
			}
		}
		input.UpdateInputs(keyboard_state);
		
		/* Update */
		mario.Update(input);


		/* Draw */
		graphics.SetViewport(Graphics::ViewportRects::screen_main_rect);
		// Fill the NES 'screen' with blue color
		graphics.BlitColoredRect(nullptr, 0x10, 0x10, 0x40, 0xFF);
		// Draw tilemap
		background_tilemap.Draw(graphics, 16, 16);
		// Draw in the player
		mario.Draw(graphics);
		// Flip to screen
		graphics.FlipRenderer();

		// Frames-per-second management
		const std::chrono::high_resolution_clock::time_point tick_end = std::chrono::high_resolution_clock::now();
		const double tick_duration = std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(tick_end - tick_start).count();
		double tick_wait = 0;
		if (FPS_LIMIT > 0) tick_wait = 1000.0 / FPS_LIMIT - tick_duration;
		if (tick_wait > 0) SDL_Delay((Uint32)tick_wait);
		// Add to window's title
		char buff[48];
		snprintf(buff, sizeof(buff), "%.0f/%.2f fps (%.4f/%.4f ms)", 1000.0 / (tick_duration + tick_wait), 1000.0 / tick_duration, tick_duration + tick_wait, tick_duration);
		std::string subtitle(buff);
		graphics.WindowSetTitle(subtitle);
	}
	return 0;
}