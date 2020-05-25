#include "game.h"

#include "constants.h"
#include "tilemap.h"
#include "sprite.h"

Game::Game() {
}

Game::~Game() {
}

int Game::Initialize() {
	if (graphics_.Initialize() < 0) {
		return -1;
	} 
	return 0;
}

int Game::Run() {

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
	Entity mario = Entity(0, 0);
	Sprite mario_spr = Sprite(graphics_, "D:/Users/Sebik/Documents/Code/Visual Studio/smb3do/content/tilesets/master.bmp", 392, 86, BLOCKSIZE_NES, BLOCKSIZE_NES);

	unsigned char background_tiles[25] = {
		0x01, 0x01, 0x01, 0x01, 0x01,
		0x01, 0x02, 0x01, 0x02, 0x01,
		0x01, 0x01, 0x01, 0x01, 0x01,
		0x00, 0x01, 0x01, 0x01, 0x00,
		0x01, 0x00, 0x00, 0x00, 0x01
	};
	Tilemap background_tilemap = Tilemap(background_tiles, 5, 5);

	bool quitGame = false;
	SDL_Event event;
	const Uint8* keyStateCurrent = SDL_GetKeyboardState(NULL);

	while (!quitGame) {
		// Handle events
		while (SDL_PollEvent(&event) != 0) {
			if (event.type == SDL_QUIT) quitGame = true;
			if (event.type == SDL_KEYDOWN) {
				if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) quitGame = true;
				if (event.key.keysym.scancode == SDL_SCANCODE_F11) graphics_.WindowToggleFullscreen();
			}
		}

		// Handle keyboard input
		if (keyStateCurrent[SDL_SCANCODE_W]) mario.pos_y_ += -0.5f;
		if (keyStateCurrent[SDL_SCANCODE_D]) mario.pos_x_ += 0.5f;
		if (keyStateCurrent[SDL_SCANCODE_S]) mario.pos_y_ += 0.5f;
		if (keyStateCurrent[SDL_SCANCODE_A]) mario.pos_x_ += -0.5f;

		// Fill the NES 'screen' with blue color
		graphics_.BlitColoredRect(NULL, 0x10, 0x10, 0x40, 0x7F);

		// Display tilemap
		background_tilemap.Draw(graphics_, 16, 16);

		// Color in the player
		SDL_Rect playerRect = { mario.pos_x_, mario.pos_y_, mario.size_x_, mario.size_y_ };
		graphics_.BlitColoredRect(&playerRect, 0xC0, 0x00, 0xC0, 0xFF);
		mario_spr.Draw(graphics_, playerRect.x, playerRect.y);
		mario_spr.Draw(graphics_, playerRect.x + 16, playerRect.y);
		mario_spr.Draw(graphics_, playerRect.x, playerRect.y + 16);

		// Flip to screen
		graphics_.FlipRenderer();
	}

	return 0;
}

void Game::Close() {
	graphics_.Close();
	SDL_Quit();
}