#include "error.h"


#include <iostream>
#include "constants.h"

void Error::PrintError(std::string& message) {
	std::cout << ANSI_COLOR_RED << "ERROR" << ANSI_COLOR_RESET ": " << message << std::endl;
}

void Error::PrintError(std::exception exception) {
	std::cout << ANSI_COLOR_RED << "ERROR" << ANSI_COLOR_RESET ": " << exception.what() << std::endl;
}

void Error::PrintWarning(std::string& message) {
	std::cout << ANSI_COLOR_YELLOW << "WARNING" << ANSI_COLOR_RESET ": " << message << std::endl;
}

void Error::PrintDebug(std::string& message) {
	if (META_DEBUG) std::cout << ANSI_COLOR_CYAN << "DEBUG" << ANSI_COLOR_RESET ": " << message << std::endl;
}