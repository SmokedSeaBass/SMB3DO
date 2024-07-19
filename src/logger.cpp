#include "logger.hpp"


#include <iostream>
#include <sstream>
#include "constants.hpp"

void Logger::PrintError(const std::string& message) {
	std::cout << ANSI_COLOR_RED << "[ERROR] " << ANSI_COLOR_RESET << message << std::endl;
}

void Logger::PrintError(const std::exception exception) {
	std::cout << ANSI_COLOR_RED << "[ERROR] " << ANSI_COLOR_RESET << exception.what() << std::endl;
}

void Logger::PrintWarning(const std::string& message) {
	std::cout << ANSI_COLOR_YELLOW << "[WARN] " << ANSI_COLOR_RESET << message << std::endl;
}

void Logger::PrintInfo(const std::string& message) {
	std::cout << ANSI_COLOR_CYAN << "[INFO] " << ANSI_COLOR_RESET << message << std::endl;
}

void Logger::PrintDebug(const std::string& message) {
	if (META_DEBUG) {
		std::cout << ANSI_COLOR_MAGENTA << "[DEBUG] " << ANSI_COLOR_RESET << message << std::endl;
	}
}

std::string Logger::PointerToString(void* ptr) {
	const void* addr = static_cast<const void*>(ptr);
	std::stringstream stream;
	stream << addr;
	return stream.str();
}
