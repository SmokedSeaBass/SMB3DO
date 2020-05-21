#include <stdio.h>
#include "SDL.h"
#include "constants.h"
#include "game.h"

int main(int argc, char* args[]) {
	printf(ANSI_COLOR_CYAN "Initializng game...\n" ANSI_COLOR_RESET);
	Game smb3do;
	if (smb3do.init() < 0) {
		printf("ERROR: Failed to initialize game\n");
		return -1;
	}

	printf("Starting main game loop...\n");
	if (smb3do.run() < 0) {
		printf("ERROR: Game was terminated early\n");
		return -1;
	}

	printf("Closing game...\n");
	smb3do.close();

	return 0;
}