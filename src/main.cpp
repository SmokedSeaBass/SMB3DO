#include <stdio.h>
#include <string>
#include "game.h"
#include "logger.h"

/*
	Super Mario Bros. 3-Do
	Copyright (C) 2019-2023 Sebastien Blazej <smokedseabass@tutanota.com>

	This is a non-profit fan-game created out of passion for Nintendo's classic NES game,
	Super Mario Bros. 3.  This fangame is NOT affiliated with or endorsed by Nintendo in
	any way. I do NOT claim any assets which are trademarked by Nintendo or their
	respective owners as my own, including but not limited to: the Super Mario trademark,
	the original game assets, fan artwork, or reference materials.

	If you like this game, please consider supporting Nintendo and buying the original game.

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

int main(int argc, char* args[]) {
	Logger::PrintDebug("Initializng game...");
	Game smb3do;
	Logger::PrintDebug("Starting main game loop...");
	if (smb3do.Run() < 0) {
		Logger::PrintWarning("Game was terminated early");
		return -1;
	}
	Logger::PrintDebug("Closing game...");
	return 0;
}