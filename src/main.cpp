#include <stdio.h>
#include <SDL.h>
#include "game.h"

int main(int argc, char* args[]) {
	printf("Initializng game...\n");
	Game smb3do;
	if (smb3do.Initialize() < 0) {
		printf("ERROR: Failed to initialize game\n");
		return -1;
	}

	printf("Starting main game loop...\n");
	if (smb3do.Run() < 0) {
		printf("ERROR: Game was terminated early\n");
		return -1;
	}

	printf("Closing game...\n");
	smb3do.Close();

	return 0;
}