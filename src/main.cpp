#include <stdio.h>
#include <string>
#include "game.h"
#include "logger.h"

/*
  Super Mario Bros. 3-Do
  Copyright (C) 2019-2022 Sebastien Blazej <smokedseabass@tutanota.com>

  This is a non-profit fan-game made out of passion for Nintendo's classic NES game,
  Super Mario Bros. 3.  This fangame is NOT affiliated with or endorsed by Nintendo in any way.
  I do NOT claim any assets which are trademarked by Nintendo or their respective owners
  as my own, including but not limited to: the Super Mario trademark and the artwork
  used in this fangame.

  If you like this game, please consider supporting Nintendo and buying the original game.
*/

int main(int argc, char* args[]) {
	Logger::PrintDebug("Initializng game...\n");
	Game smb3do;

	Logger::PrintDebug("Starting main game loop...\n");
	if (smb3do.Run() < 0) {
		Logger::PrintWarning("Game was terminated early");
		return -1;
	}

	Logger::PrintDebug("Closing game...\n");

	return 0;
}