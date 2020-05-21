#include "game.h"

#include "constants.h"

Game::Game() {
}

Game::~Game() {
}

int Game::init() {
	if (graphics.init() < 0) {
		return -1;
	}
	return 0;
}

int Game::run() {

	class Entity {
	private:

	public:
		float posX;
		float posY;
		float sizeX;
		float sizeY;

		Entity(float x, float y) {
			this->posX = x;
			this->posY = y;
			this->sizeX = BLOCKSIZE_NES;
			this->sizeY = BLOCKSIZE_NES;
		}
	};
	Entity mario = Entity(0, 0);

	bool quitGame = false;
	SDL_Event event;

	while (!quitGame) {
		// Handle events
		while (SDL_PollEvent(&event) != 0) {
			if (event.type == SDL_QUIT) quitGame = true;
			if (event.type == SDL_KEYDOWN) {
				if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) quitGame = true;
			}
		}

		// Handle keyboard input
		const Uint8* keyStateCurrent = SDL_GetKeyboardState(NULL);  // TODO: Capture only used input
		if (keyStateCurrent[SDL_SCANCODE_W]) mario.posY += -0.05f;
		if (keyStateCurrent[SDL_SCANCODE_D]) mario.posX += 0.05f;
		if (keyStateCurrent[SDL_SCANCODE_S]) mario.posY += 0.05f;
		if (keyStateCurrent[SDL_SCANCODE_A]) mario.posX += -0.05f;

		// Fill the NES 'screen' with blue color
		SDL_Rect playScreen = { 0, 0, WINDOW_WIDTH_NES, WINDOW_HEIGHT_NES };
		graphics.renderColoredRect(&playScreen, 0x10, 0x10, 0x40, 0x00);

		// Color in the player
		SDL_Rect playerRect = { mario.posX, mario.posY, mario.sizeX, mario.sizeY };
		graphics.renderColoredRect(&playerRect, 0x00, 0x00, 0xC0, 0x00);

		// Flip to screen
		graphics.flipRenderer();
	}

	return 0;
}

void Game::close() {
	graphics.close();
	SDL_Quit();
}