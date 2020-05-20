#include <stdio.h>
#include "SDL.h"
#include "constants.h"

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
		this->sizeX = BLOCKSIZE;
		this->sizeY = BLOCKSIZE;
	}
};

SDL_Window* windowMain = NULL;

SDL_Renderer* rendererMain = NULL;


int gameInit() {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("ERROR: SDL could not initialize: %s\n", SDL_GetError());
		return -1;
	}
	windowMain = SDL_CreateWindow("SMB3DO", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
	if (windowMain == NULL) {
		printf("ERROR: Main window could not be created: %s\n", SDL_GetError());
		return -1;
	}
	rendererMain = SDL_CreateRenderer(windowMain, -1, SDL_RENDERER_ACCELERATED);
	if (rendererMain == NULL) {
		printf("ERROR: Main renderer could not be created: %s\n", SDL_GetError());
		return -1;
	}
	return 0;
}

int gameLoop() {
	bool quitGame = false;
	SDL_Event e;

	Entity mario = Entity(0, 0);

	while (!quitGame) {
		// Handle events
		while (SDL_PollEvent(&e) != 0) {
			if (e.type == SDL_QUIT) quitGame = true;
			if (e.type == SDL_KEYDOWN) {
				if (e.key.keysym.scancode == SDL_SCANCODE_ESCAPE) quitGame = true;
			}
		}

		// Handle keyboard input
		const Uint8* keyStateCurrent = SDL_GetKeyboardState(NULL);
		if (keyStateCurrent[SDL_SCANCODE_W]) mario.posY += -0.05 * SCREEN_SCALE;
		if (keyStateCurrent[SDL_SCANCODE_D]) mario.posX += 0.05 * SCREEN_SCALE;
		if (keyStateCurrent[SDL_SCANCODE_S]) mario.posY += 0.05 * SCREEN_SCALE;
		if (keyStateCurrent[SDL_SCANCODE_A]) mario.posX += -0.05 * SCREEN_SCALE;

		// Fill the surface with black color and the the SNES 'playscreen' with white color
		SDL_SetRenderDrawColor(rendererMain, 0x00, 0x00, 0x00, 0x00);
		SDL_RenderFillRect(rendererMain, NULL);
		int playScreenPosX = (WINDOW_WIDTH - SCREEN_WIDTH) / 2;
		int playScreenPosY = (WINDOW_HEIGHT - SCREEN_HEIGHT) / 2;
		SDL_Rect playScreen = { playScreenPosX, playScreenPosY, SCREEN_WIDTH, SCREEN_HEIGHT };
		SDL_SetRenderDrawColor(rendererMain, 0x10, 0x10, 0x40, 0x00);
		SDL_RenderFillRect(rendererMain, &playScreen);

		// Color in the player
		SDL_Rect playerRect = { mario.posX, mario.posY, mario.sizeX, mario.sizeY };
		SDL_SetRenderDrawColor(rendererMain, 0x00, 0x00, 0xC0, 0x00);
		SDL_RenderFillRect(rendererMain, &playerRect);

		// Update the surface
		SDL_RenderPresent(rendererMain);
	}

	return 0;
}

int gameClose() {
	SDL_DestroyRenderer(rendererMain);
	SDL_DestroyWindow(windowMain);
	SDL_Quit();

	return 0;
}


int main(int argc, char* args[]) {
	printf("Initializng game...\n");
	if (gameInit() < 0) {
		printf("ERROR: Failed to initialize game\n");
		return -1;
	}
	printf("Starting main game loop...\n");
	if (gameLoop() < 0) {
		printf("ERROR: Game loop exited early\n");
		return -1;
	}
	printf("Closing game...\n");
	if (gameClose() < 0) {
		printf("ERROR: Failed to deallocate resources properly and shutdown SDL\n");
		return -1;
	}

	return 0;
}