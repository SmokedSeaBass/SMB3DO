#include "error.h"


#include <iostream>
#include <sstream>
#include "constants.h"

void Error::PrintError(const std::string& message) {
	std::cout << ANSI_COLOR_RED << "ERROR" << ANSI_COLOR_RESET ": " << message << std::endl;
}

void Error::PrintError(const std::exception exception) {
	std::cout << ANSI_COLOR_RED << "ERROR" << ANSI_COLOR_RESET ": " << exception.what() << std::endl;
}

void Error::PrintWarning(const std::string& message) {
	std::cout << ANSI_COLOR_YELLOW << "WARNING" << ANSI_COLOR_RESET ": " << message << std::endl;
}

void Error::PrintDebug(const std::string& message) {
	if (META_DEBUG) std::cout << ANSI_COLOR_CYAN << "DEBUG" << ANSI_COLOR_RESET ": " << message << std::endl;
}

std::string Error::ptr_to_string(void* ptr) {
	const void* addr = static_cast<const void*>(ptr);
	std::stringstream ss;
	ss << addr;
	return ss.str();
}
