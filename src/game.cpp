#include "game.h"

#include <chrono>
#include "constants.h"
#include "tilemap.h"
#include "sprite.h"
#include "animated_sprite.h"
#include "player.h"
#include "input.h"
#include "error.h"

Game::Game() { }

Game::~Game() { }

int Game::Run() {
	Graphics graphics;
	if (graphics.Initialize() < 0) {
		Error::PrintError(std::runtime_error("Could not initialize graphics"));
		return -1;
	}
	Input input;
	bool quitGame = false;
	SDL_Event event;
	const Uint8* keyboard_state = SDL_GetKeyboardState(nullptr);

	double tick_duration = 0;
	double tick_wait = 0;
	
	// Test creation code
	// Mario
	Sprite mario_spr = Sprite(graphics, "assets/sprite_sheets/mario_8bit.bmp", 0, 0, 215, 88, static_cast<int>(round(BLOCKSIZE_NES)), static_cast<int>(round(BLOCKSIZE_NES)));
	mario_spr.SetOrigin(Sprite::ORIGIN_ORIENTATION::BOTTOM_MIDDLE);
	Player mario = Player(&mario_spr, 32.0, 32.0);
	// Tileset
	Sprite debug_tileset_sprite = Sprite(graphics, "assets/tilesets/debug.bmp");
	Tileset debug_tileset = Tileset(&debug_tileset_sprite, 16, 16, 0, 0);
	// Animated Question Block
	AnimatedSprite question_block = AnimatedSprite(graphics, "assets/tilesets/question_block.bmp", -1, -1, 0, 0, BLOCKSIZE_NES, BLOCKSIZE_NES, 8.0, 4, 1);
	// Tilemap
	std::vector<std::vector<unsigned int>> background_tiles_2 = {
		{0, 1, 1, 1, 0},
		{1, 17, 1, 17, 1},
		{1, 1, 1, 1, 1},
		{17, 1, 1, 1, 17},
		{1, 17, 17, 17, 1},
		{0, 1, 1, 1, 0}
	};
	Tilemap background_tilemap = Tilemap(background_tiles_2, &debug_tileset);
	Tilemap test_tilemap = Tilemap("assets/maps/test.tmx");
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
				if (event.key.keysym.scancode == SDL_SCANCODE_F11) graphics.WindowToggleFullscreen();
			}
		}
		input.UpdateInputs(keyboard_state);
		
		/* Update */
		mario.Update(input);
		question_block.Update(tick_duration + tick_wait);


		/* Draw */
		graphics.SetViewport(Graphics::ViewportRects::screen_main_rect);
		// Fill the NES 'screen' with blue color
		graphics.BlitColoredRect(nullptr, 0x10, 0x10, 0x40, 0xFF);
		// Draw tilemap
		//background_tilemap.Draw(graphics, 16, 16);
		test_tilemap.Draw(graphics, 0, 0);
		// Objects
		//question_block.Draw(graphics, 16, 128);
		// Draw in the player
		mario.Draw(graphics);
		// Flip to screen
		graphics.FlipRenderer();

		// Frames-per-second management
		const std::chrono::high_resolution_clock::time_point tick_end = std::chrono::high_resolution_clock::now();
		tick_duration = std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(tick_end - tick_start).count();
		if (FPS_LIMIT > 0) tick_wait = 1000.0 / FPS_LIMIT - tick_duration;
		if (tick_wait > 0) SDL_Delay((Uint32)tick_wait);
		// Add to window's title
		char title_buff[48];
		snprintf(title_buff, sizeof(title_buff), "%.0f/%.2f fps (%.4f/%.4f ms)", 1000.0 / (tick_duration + tick_wait), 1000.0 / tick_duration, tick_duration + tick_wait, tick_duration);
		std::string subtitle(title_buff);
		graphics.WindowSetTitle(subtitle);
	}
	return 0;
}