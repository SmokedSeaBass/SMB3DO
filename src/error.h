#pragma once

#include <string>
#include <stdexcept>

struct Error {
public:
	static void PrintError(std::string& message);
	static void PrintError(std::exception exception);
	static void PrintWarning(std::string& message);
	static void PrintDebug(std::string& message);
};