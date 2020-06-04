#include "game.h"

#include "constants.h"
#include "tilemap.h"
#include "sprite.h"
#include "player.h"
#include "input.h"

Game::Game() {
}

Game::~Game() {
}

int Game::Run() {
	Graphics graphics;
	if (graphics.Initialize() < 0) {
		printf("ERROR: Grpahics could not initialize: %s\n", SDL_GetError());
		return -1;
	}
	Input input;
	bool quitGame = false;
	SDL_Event event;
	const Uint8* keyboard_state = SDL_GetKeyboardState(NULL);
	
	// Testing code
	// Mario
	Sprite mario_spr = Sprite(graphics, "content/sprite_sheets/mario_8bit.bmp", 0, 0, 215, 88, BLOCKSIZE_NES, BLOCKSIZE_NES);
	mario_spr.SetOrigin(Sprite::ORIGIN_ORIENTATION::BOTTOM_MIDDLE);
	Player mario = Player(&mario_spr, 32.0, 32.0);
	// Tileset
	Sprite tileset_debug = Sprite(graphics, "content/tilesets/debug.bmp");
	Tile air = Tile(0x00, &tileset_debug, { 0, 0, 16, 16 }, "air");
	Tile wood = Tile(0x01, &tileset_debug, { 0, 16, 16, 16 }, "wood");
	Tile* background_tiles[30] = {
		nullptr, &air, & air, &air, nullptr,
		&air, &wood, & air, &wood, &air,
		&air, &air, & air, &air, &air,
		&wood, &air, & air, &air, &wood,
		&air, &wood, & wood, &wood, &air,
		nullptr, &air, & air, &air, nullptr
	};
	Tilemap background_tilemap = Tilemap(background_tiles, 5, 6);
	
	// Main game loop
	while (!quitGame) {
		/* Get Input */
		// Handle events
		while (SDL_PollEvent(&event) != 0) {
			if (event.type == SDL_QUIT) quitGame = true;
			if (event.type == SDL_KEYDOWN) {
				if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) quitGame = true;
				if (event.key.keysym.scancode == SDL_SCANCODE_F11) graphics.WindowToggleFullscreen();
			}
		}
		input.UpdateInputs(keyboard_state);
		
		/* Update */
		mario.Update(input);

		/* Draw */
		// Fill the NES 'screen' with blue color
		graphics.BlitColoredRect(nullptr, 0x10, 0x10, 0x40, 0xFF);
		// Draw tilemap
		background_tilemap.Draw(graphics, 16, 16);
		// Draw in the player
		mario.Draw(graphics);

		// Flip to screen
		graphics.FlipRenderer();
	}
	return 0;
}