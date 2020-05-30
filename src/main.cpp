#include <stdio.h>
#include "game.h"

/*
  Super Mario Bros. 3-Do
  Copyright (C) 2019-2020 Sebastien Blazej <smokedseabass@gmail.com>

  This is a non-profit fangame made out of passion for Nintendo's classic NES game,
  Super Mario Bros. 3.  This fangame is NOT affiliated with or endorsed by Nintendo.
  I do NOT claim any assets which are trademarked by Nintendo or their respective owners
  as my own, including but not limited to: the Super Mario trademark and the artwork
  used in this fangame.

  If you like this game, please consider supporting Nintendo and buying the original game.
*/

int main(int argc, char* args[]) {
	printf("Initializng game...\n");
	Game smb3do;

	printf("Starting main game loop...\n");
	if (smb3do.Run() < 0) {
		printf("ERROR: Game was terminated early\n");
		return -1;
	}

	printf("Closing game...\n");

	return 0;
}