#pragma once

#include <string>
#include <stdexcept>

struct Logger {
public:
	static void PrintError(const std::string& message);
	static void PrintError(const std::exception exception);
	static void PrintWarning(const std::string& message);
	static void PrintInfo(const std::string& message);
	static void PrintDebug(const std::string& message);
	static std::string PointerToString(void* ptr);
};