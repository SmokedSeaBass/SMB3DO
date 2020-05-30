#include "game.h"

#include "constants.h"
#include "tilemap.h"
#include "sprite.h"

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

	// to be moved to its own class
	class Entity {
	private:

	public:
		double pos_x_;
		double pos_y_;
		double size_x_;
		double size_y_;

		Entity(float x, float y) {
			this->pos_x_ = x;
			this->pos_y_ = y;
			this->size_x_ = BLOCKSIZE_NES;
			this->size_y_ = BLOCKSIZE_NES;
		}
	};
	Entity mario = Entity(WINDOW_WIDTH_NES/2.0, WINDOW_HEIGHT_NES/2.0);
	Sprite mario_spr = Sprite(graphics, "content/tilesets/debug.bmp", 0, 0, 16, 0, BLOCKSIZE_NES, BLOCKSIZE_NES);

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

	bool quitGame = false;
	SDL_Event event;
	const Uint8* keyStateCurrent = SDL_GetKeyboardState(NULL);

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
		
		/* Update */
		// Handle keyboard input
		if (keyStateCurrent[SDL_SCANCODE_W]) mario.pos_y_ += -1.0f;
		if (keyStateCurrent[SDL_SCANCODE_D]) mario.pos_x_ += 1.0f;
		if (keyStateCurrent[SDL_SCANCODE_S]) mario.pos_y_ += 1.0f;
		if (keyStateCurrent[SDL_SCANCODE_A]) mario.pos_x_ += -1.0f;

		/* Draw */
		// Fill the NES 'screen' with blue color
		graphics.BlitColoredRect(NULL, 0x10, 0x10, 0x40, 0xFF);
		// Display tilemap
		background_tilemap.Draw(graphics, 16, 16);
		// Color in the player
		SDL_Rect playerRect = { mario.pos_x_, mario.pos_y_, mario.size_x_, mario.size_y_ };
		//graphics.BlitColoredRect(&playerRect, 0xC0, 0x00, 0xC0, 0xFF);
		mario_spr.Draw(graphics, playerRect.x, playerRect.y, { 16, 0, 16, 16 });
		mario_spr.SetRect(32, 0, 16, 16);
		mario_spr.Draw(graphics, playerRect.x + 16, playerRect.y);
		mario_spr.Draw(graphics, playerRect.x, playerRect.y + 16);

		// Flip to screen
		graphics.FlipRenderer();
	}
	return 0;
}