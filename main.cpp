#include <stdio.h>
#include "SDL.h"
#include "SDL_image.h"


// Global Constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 360;

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
		this->sizeX = 16.0;
		this->sizeY = 16.0;
	}
};


int main(int argc, char* args[]) {
	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("ERROR: SDL could not initialize: %s\n", SDL_GetError());
		return -1;
	}

	// The surface contained by the window
	SDL_Surface* screenSurface = NULL;

	// Create the window we will render to
	SDL_Window* mainWindow = SDL_CreateWindow("SMB3DO", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (mainWindow == NULL) {
		printf("ERROR: Window could not be created: %s\n", SDL_GetError());
		return -1;
	}

	// Misc variables
	bool quit = false;
	Entity mario = Entity(0, 0);

	// Game loop
	SDL_Event e;
	while ( !quit ) {
		// Handle all events
		while ( SDL_PollEvent(&e) != 0 ) {
			// User requests to quit the program
			if (e.type == SDL_QUIT) {
				quit = true;
			}
		}
		// Handle keyboard input
		const Uint8* keyStateCurrent = SDL_GetKeyboardState(NULL);
		if ( keyStateCurrent[SDL_SCANCODE_W] ) { mario.posY += -0.05; }
		if ( keyStateCurrent[SDL_SCANCODE_D] ) { mario.posX += 0.05; }
		if ( keyStateCurrent[SDL_SCANCODE_S] ) { mario.posY += 0.05; }
		if ( keyStateCurrent[SDL_SCANCODE_A] ) { mario.posX += -0.05; }

		// Get window surface
		screenSurface = SDL_GetWindowSurface(mainWindow);

		// Fill the surface with white color
		SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0x00, 0x00, 0x00));

		// Color in the player
		SDL_Rect playerRect = { mario.posX, mario.posY, mario.sizeX, mario.sizeY };
		SDL_FillRect(screenSurface, &playerRect, SDL_MapRGB(screenSurface->format, 0x00, 0x00, 0xF0));

		// Update the surface
		SDL_UpdateWindowSurface(mainWindow);
	}
}